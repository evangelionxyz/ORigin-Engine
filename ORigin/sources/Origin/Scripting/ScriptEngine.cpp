// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"
#include "ScriptClass.h"
#include "Origin/Core/ConsoleManager.h"

#include "Origin/Scene/Components/Components.h"
#include "Origin/Project/Project.h"
#include "Origin/Core/Application.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>
#include <cstdlib>
#include <FileWatch.hpp>

namespace origin
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{"System.Boolean",       ScriptFieldType::Bool},
		{"System.Single",        ScriptFieldType::Float},
		{"System.Char",          ScriptFieldType::Char},
		{"System.Byte",          ScriptFieldType::Byte},
		{"System.Double",        ScriptFieldType::Double},
		{"System.Int16",         ScriptFieldType::Short},
		{"System.Int32",         ScriptFieldType::Int},
		{"System.Int64",         ScriptFieldType::Long},
		{"System.UInt16",        ScriptFieldType::UShort},
		{"System.UInt32",        ScriptFieldType::UInt},
		{"System.UInt64",        ScriptFieldType::ULong},
		{"System.UInt",          ScriptFieldType::UByte},
		{"ORiginEngine.Vector2", ScriptFieldType::Vector2},
		{"ORiginEngine.Vector3", ScriptFieldType::Vector3},
		{"ORiginEngine.Vector4", ScriptFieldType::Vector4},
		{"ORiginEngine.Entity",	 ScriptFieldType::Entity},
	};

	namespace Utils
	{
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			OGN_PROFILER_FUNCTION();

			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			// failed to open file
			if (!stream)
				return nullptr;

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			// file is empty
			if (size == 0)
				return nullptr;

			auto* buffer = new char[size];
			stream.read((char*)buffer, size);

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& filepath)
		{
			OGN_PROFILER_FUNCTION();

			uint32_t fileSize = 0;
			char* fileData = ReadBytes(filepath, &fileSize);

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				return nullptr;
			}
			std::string assemblyPath = filepath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			OGN_PROFILER_FUNCTION();

			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				OGN_CORE_TRACE("{0}.{1}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			OGN_PROFILER_LOGIC();

			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				OGN_CORE_ERROR("Unkown Field Type : {}", typeName);
				return ScriptFieldType::Invalid;
			}
			return it->second;
		}
	}

    struct ScriptEngineData
    {
        MonoDomain *RootDomain = nullptr;
        MonoDomain *AppDomain = nullptr;

        MonoAssembly *CoreAssembly = nullptr;
        MonoImage *CoreAssemblyImage = nullptr;

        MonoAssembly *AppAssembly = nullptr;
        MonoImage *AppAssemblyImage = nullptr;

        ScriptClass EntityClass;
        std::vector<std::string> EntityScriptStorage;

        std::filesystem::path CoreAssemblyFilepath;
        std::filesystem::path AppAssemblyFilepath;

        std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFilewatcher;
        bool AssemblyReloadingPending = false;

        Scene *SceneContext = nullptr;
        std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
        std::unordered_map<UUID, std::shared_ptr<ScriptInstance>> EntityInstances;
        std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;
    };

    ScriptEngineData *s_ScriptEngineData = nullptr;

	void ScriptEngine::InitMono()
	{
		OGN_PROFILER_LOGIC();
#ifdef _WIN32
    	mono_set_assemblies_path("lib");
#elif __linux__
    	mono_set_assemblies_path("/usr/lib");
		mono_set_dirs("/usr/lib", "/etc/mono");
		setenv("LD_LIBRARY_PATH", "/usr/lib", 1);
#endif
		MonoDomain *rootDomain = mono_jit_init("ORiginJITRuntime");
		OGN_CORE_ASSERT(rootDomain, "[Script Engine] Mono Domain is NULL!");
		s_ScriptEngineData->RootDomain = rootDomain;
		OGN_CORE_TRACE("[Script Engine] MONO Initialized");
		const char *mono_version = mono_get_runtime_build_info();
		OGN_CORE_TRACE("[Script Engine] MONO Version: {0}", mono_version);
	}

	void ScriptEngine::ShutdownMono()
	{
		OGN_PROFILER_LOGIC();

		if (!s_ScriptEngineData)
			return;

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_ScriptEngineData->AppDomain);
		s_ScriptEngineData->AppDomain = nullptr;
		s_ScriptEngineData->CoreAssembly = nullptr;

		mono_jit_cleanup(s_ScriptEngineData->RootDomain);
		s_ScriptEngineData->RootDomain = nullptr;

		OGN_CORE_TRACE("[Script Engine] Mono  Shutdown");
	}

	void ScriptEngine::Init()
	{
		OGN_PROFILER_LOGIC();

		std::filesystem::path appAssemblyPath = Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;

		if (s_ScriptEngineData)
		{
			s_ScriptEngineData->AppAssemblyFilepath = appAssemblyPath.generic_string();

			ReloadAssembly();
			return;
		}

		s_ScriptEngineData = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		// Script Core Assembly
		OGN_CORE_ASSERT(std::filesystem::exists("Resources/ScriptCore/ORigin-ScriptCore.dll"), "[Script Engine] Script core assembly not found!");
		LoadAssembly("Resources/ScriptCore/ORigin-ScriptCore.dll");
		LoadAppAssembly(appAssemblyPath);
		LoadAssemblyClasses();

		// storing classes name into storage
		for (auto &it : s_ScriptEngineData->EntityClasses)
			s_ScriptEngineData->EntityScriptStorage.emplace_back(it.first);

		s_ScriptEngineData->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
		ScriptGlue::RegisterComponents();

		OGN_CORE_TRACE("[Script Engine]: Initialized");
	}

	void ScriptEngine::Shutdown()
	{
		OGN_PROFILER_LOGIC();

		ShutdownMono();
		if (!s_ScriptEngineData)
			return;
		s_ScriptEngineData->EntityClasses.clear();
		s_ScriptEngineData->EntityInstances.clear();

		delete s_ScriptEngineData;

		OGN_CORE_TRACE("[Script Engine] Shutdown");
	}

	bool ScriptEngine::LoadAssembly(const std::filesystem::path &filepath)
	{
		OGN_PROFILER_LOGIC();

		char *domain_name = new char[20];
		strcpy(domain_name, "ORiginScriptRuntime");
		s_ScriptEngineData->AppDomain = mono_domain_create_appdomain(domain_name, nullptr);
		delete[] domain_name;

		mono_domain_set(s_ScriptEngineData->AppDomain, true);

		s_ScriptEngineData->CoreAssemblyFilepath = filepath;

		s_ScriptEngineData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		if (s_ScriptEngineData->CoreAssembly == nullptr)
			return false;

		s_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->CoreAssembly);
		return true;
	}

	static void OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event change_type)
	{
		OGN_PROFILER_LOGIC();

		if (!s_ScriptEngineData->AssemblyReloadingPending && change_type == filewatch::Event::modified)
		{
			s_ScriptEngineData->AssemblyReloadingPending = true;

			Application::Get().SubmitToMainThread([]()
			{
				s_ScriptEngineData->AppAssemblyFilewatcher.reset();
				ScriptEngine::ReloadAssembly();
			});
		}
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path &filepath)
	{
		OGN_PROFILER_LOGIC();

		s_ScriptEngineData->AppAssemblyFilepath = filepath;

		s_ScriptEngineData->AppAssembly = Utils::LoadMonoAssembly(filepath);
		if (!s_ScriptEngineData->AppAssembly)
		{
			OGN_CORE_ASSERT(false, "[Script Engine] App Assembly is empty {0}", filepath);
			return false;
		}

		s_ScriptEngineData->AppAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->AppAssembly);
		s_ScriptEngineData->AppAssemblyFilewatcher = std::make_unique<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_ScriptEngineData->AssemblyReloadingPending = false;

		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		OGN_PROFILER_LOGIC();

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_ScriptEngineData->AppDomain);
		LoadAssembly(s_ScriptEngineData->CoreAssemblyFilepath);

		if (LoadAppAssembly(s_ScriptEngineData->AppAssemblyFilepath))
		{
			LoadAssemblyClasses();

			// storing classes name into storage
			s_ScriptEngineData->EntityScriptStorage.clear();

			for (auto &it : s_ScriptEngineData->EntityClasses)
			{
				s_ScriptEngineData->EntityScriptStorage.emplace_back(it.first);
			}

			// retrieve entity class
			s_ScriptEngineData->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
			ScriptGlue::RegisterComponents();

			OGN_CORE_TRACE("[Script Engine] Assembly Reloaded");
			PUSH_CONSOLE_INFO("[Script Engine] Assembly Reloaded");
		}
	}

	void ScriptEngine::SetSceneContext(Scene *scene)
	{
		OGN_PROFILER_LOGIC();

		s_ScriptEngineData->SceneContext = scene;
	}

	void ScriptEngine::ClearSceneContext()
	{
		OGN_PROFILER_LOGIC();

		s_ScriptEngineData->SceneContext = nullptr;
		s_ScriptEngineData->EntityInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string &fullClassName)
	{
		OGN_PROFILER_LOGIC();
		if(s_ScriptEngineData)
		{
			return s_ScriptEngineData->EntityClasses.find(fullClassName) != s_ScriptEngineData->EntityClasses.end();
		}
		return false;		
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		OGN_PROFILER_LOGIC();

		auto &sc = entity.GetComponent<ScriptComponent>();
		if (EntityClassExists(sc.ClassName))
		{
			UUID entityID = entity.GetUUID();

			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_ScriptEngineData->EntityClasses[sc.ClassName], entity);
			s_ScriptEngineData->EntityInstances[entityID] = instance;

			// Copy Fields Value from Editor to Runtime
			if (s_ScriptEngineData->EntityScriptFields.find(entityID) != s_ScriptEngineData->EntityScriptFields.end())
			{
				ScriptFieldMap &fieldMap = s_ScriptEngineData->EntityScriptFields.at(entityID);
				const auto classFields = instance->GetScriptClass()->GetFields();

				if (fieldMap.size() != classFields.size())
				{
                    // find any private members
                    std::vector<std::string> keysToRemove;
                    for (const auto &[fieldName, field] : fieldMap)
                    {
                        if (classFields.find(fieldName) == classFields.end())
                        {
                            keysToRemove.push_back(fieldName);
                        }
                    }

                    // remove if they are there
                    for (const auto &key : keysToRemove)
                    {
                        fieldMap.erase(key);
                    }
				}
				
				for (auto &[name, fieldInstance] : fieldMap)
				{
					// Check invalid type
					if (fieldInstance.Field.Type == ScriptFieldType::Invalid)
					{
						ScriptFieldType type = instance->GetScriptClass()->GetFields()[name].Type;
						fieldInstance.Field.Type = type;
						OGN_CORE_WARN("[Script Engine] Checking invalid type {0}", name);
						PUSH_CONSOLE_WARNING("[Script Engine] Checking invalid type {0}", name);
					}

					switch (fieldInstance.Field.Type)
					{
					case ScriptFieldType::Entity:
					{
						uint64_t uuid = *(uint64_t *)fieldInstance.m_Buffer;
						if (uuid == 0)
						{
							OGN_CORE_ERROR("[Script Engine] Field '{0}' (Entity class) is not assigned yet", name);
							PUSH_CONSOLE_ERROR("[Script Engine] Field '{0}' (Entity class) is not assigned yet", name);
							continue;
						}

						MonoMethod *ctorMethod = s_ScriptEngineData->EntityClass.GetMethod(".ctor", 1);
						if (!ctorMethod)
						{
							OGN_CORE_ERROR("[Script Engine] Failed to find constructor");
							PUSH_CONSOLE_ERROR("[Script Engine] Failed to find constructor {0}", name);
							continue;
						}

						// Create new instance
						MonoObject *entityInstance = ScriptEngine::InstantiateObject(s_ScriptEngineData->EntityClass.m_MonoClass);
						if (!entityInstance)
						{
							OGN_CORE_ERROR("[Script Engine] Failed to create Entity instance.");
							PUSH_CONSOLE_ERROR("[Script Engine] Failed to create Entity instance. {0}", name);
							continue;
						}

						// Set the entity ID
						void *param = &uuid;
						mono_runtime_invoke(ctorMethod, entityInstance, &param, nullptr);

						// set the new instance into the app class's field
						instance->SetFieldValueInternal(name, entityInstance);
						break;
					}
					case ScriptFieldType::Invalid:
						PUSH_CONSOLE_ERROR("[Script Engine] Null Object Field {0}", name);
						OGN_CORE_ASSERT(false, "[Script Engine] Null Object Field {0}", name);
						return;
                    default:
                        instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
                        break;
					}
				}
			}

			// C# On Create Function
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float time)
	{
		OGN_PROFILER_LOGIC();

		UUID entityID = entity.GetUUID();
		const auto &it = s_ScriptEngineData->EntityInstances.find(entityID);
		if (it == s_ScriptEngineData->EntityInstances.end())
		{
			//OGN_CORE_ERROR("[Script Engine] Entity script instance is not attached! {0}", entity.GetTag(), entityID);
			//PUSH_CONSOLE_ERROR("[Script Engine] Entity script instance is not attached! {0}", entityID);
			return;
		}

		std::shared_ptr<ScriptInstance> instance = s_ScriptEngineData->EntityInstances.at(entityID);
		instance->InvokeOnUpdate(time);
	}

	MonoString *ScriptEngine::CreateString(const char *string)
	{
		return mono_string_new(s_ScriptEngineData->AppDomain, string);
	}

    ScriptClass *ScriptEngine::GetEntityClass()
    {
		return &s_ScriptEngineData->EntityClass;
    }

    std::shared_ptr<ScriptClass> ScriptEngine::GetEntityClassesByName(const std::string &name)
	{
		if (s_ScriptEngineData->EntityClasses.find(name) == s_ScriptEngineData->EntityClasses.end())
			return nullptr;

		return s_ScriptEngineData->EntityClasses.at(name);
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_ScriptEngineData->EntityClasses;
	}

	ScriptFieldMap &ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		OGN_PROFILER_LOGIC();

		OGN_CORE_ASSERT(entity.IsValid(), "[Script Engine] Failed to get entity");

		UUID entityID = entity.GetUUID();
		return s_ScriptEngineData->EntityScriptFields[entityID];
	}

	std::vector<std::string> ScriptEngine::GetScriptClassStorage()
	{
		return s_ScriptEngineData->EntityScriptStorage;
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID uuid)
	{
		OGN_PROFILER_LOGIC();

		const auto &it = s_ScriptEngineData->EntityInstances.find(uuid);
		if (it == s_ScriptEngineData->EntityInstances.end())
		{
			OGN_CORE_ERROR("[Script Engine] Failed to find {0}", uuid);
			PUSH_CONSOLE_ERROR("[Script Engine] Failed to find {0} ", (uint64_t)uuid);
			return nullptr;
		}

		return it->second;
	}

	Scene *ScriptEngine::GetSceneContext()
	{
		return s_ScriptEngineData->SceneContext;
	}

	MonoImage *ScriptEngine::GetCoreAssemblyImage()
	{
		return s_ScriptEngineData->CoreAssemblyImage;
	}

    MonoImage *ScriptEngine::GetAppAssemblyImage()
    {
		return s_ScriptEngineData->AppAssemblyImage;
    }

    MonoObject *ScriptEngine::GetManagedInstance(UUID uuid)
	{
		OGN_PROFILER_LOGIC();

		if (s_ScriptEngineData->EntityInstances.find(uuid) == s_ScriptEngineData->EntityInstances.end())
		{
			PUSH_CONSOLE_ERROR("[Script Engine] Invalid Script Instance {0}", (uint64_t)uuid);
			OGN_CORE_ASSERT(false, "[Script Engine] Invalid Script Instance");
		}

		return s_ScriptEngineData->EntityInstances.at(uuid)->GetMonoObject();
	}

	MonoObject *ScriptEngine::InstantiateObject(MonoClass *monoClass)
	{
		OGN_PROFILER_LOGIC();

		MonoObject *instance = mono_object_new(s_ScriptEngineData->AppDomain, monoClass);
		if (!instance)
		{
			return nullptr;
		}

		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		OGN_PROFILER_LOGIC();

		s_ScriptEngineData->EntityClasses.clear();

		const MonoTableInfo *typeDefinitionTable = mono_image_get_table_info(s_ScriptEngineData->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionTable);
		MonoClass *entityClass = mono_class_from_name(s_ScriptEngineData->CoreAssemblyImage, "ORiginEngine", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

			const char *nameSpace = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char *className = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (strlen(nameSpace) != 0)
			{
				fullName = fmt::format("{}.{}", nameSpace, className);
			}
			else
			{
				fullName = className;
			}

			MonoClass *monoClass = mono_class_from_name(s_ScriptEngineData->AppAssemblyImage, nameSpace, className);

			if (monoClass == entityClass) continue;

			const bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity) continue;

			std::shared_ptr<ScriptClass> scriptClass = std::make_shared<ScriptClass>(nameSpace, className);
			s_ScriptEngineData->EntityClasses[fullName] = scriptClass;

			int fieldCount = mono_class_num_fields(monoClass);
			void *iterator = nullptr;

			while (MonoClassField *field = mono_class_get_fields(monoClass, &iterator))
			{
				const char *fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType *type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}

		auto &entityClasses = s_ScriptEngineData->EntityClasses;
	}
}