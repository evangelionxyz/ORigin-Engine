// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"
#include "ScriptClass.h"
#include "Origin/Core/ConsoleManager.h"

#include "Origin/Scene/Components/Components.h"
#include "Origin/Project/Project.h"
#include "Origin/Core/Application.h"
#include "Origin/Utils/Utils.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/debug-helpers.h>
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
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* out_size)
		{
			OGN_PROFILER_FUNCTION();

			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			// failed to open file
			if (!stream)
				return nullptr;

			const size_t end = stream.tellg();
			stream.seekg(0, std::ios::beg);

			const u32 size = end - stream.tellg();

			// file is empty
			if (!size)
			{
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read(static_cast<std::istream::char_type*>(reinterpret_cast<void *>(buffer)), size);

			*out_size = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& filepath)
		{
			OGN_PROFILER_FUNCTION();

			uint32_t fileSize = 0;
			char* file_data = ReadBytes(filepath, &fileSize);

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(file_data, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* error_message = mono_image_strerror(status);
				OGN_CORE_ERROR("{}", error_message);
				return nullptr;
			}

			const std::string &assembly_path = filepath.generic_string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, assembly_path.c_str(), &status, 0);
			mono_image_close(image);

			// don't forget to free the file data
			delete[] file_data;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			OGN_PROFILER_FUNCTION();

			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* type_definition_table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			const i32 num_types = mono_table_info_get_rows(type_definition_table);

			for (i32 i = 0; i < num_types; i++)
			{
				u32 cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(type_definition_table, i, cols, MONO_TYPEDEF_SIZE);

				const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				OGN_CORE_TRACE("{0}.{1}", name_space, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* mono_type)
		{
			OGN_PROFILER_LOGIC();

			std::string type_name = mono_type_get_name(mono_type);

			const auto it = s_ScriptFieldTypeMap.find(type_name);
			if (it == s_ScriptFieldTypeMap.end())
			{
				OGN_CORE_ERROR("Unkown Field Type : {}", type_name);
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
    	mono_set_assemblies_path("Lib");
#elif __linux__
    	mono_set_assemblies_path("/usr/lib");
		mono_set_dirs("/usr/lib", "/etc/mono");
		setenv("LD_LIBRARY_PATH", "/usr/lib", 1);
#endif
		MonoDomain *root_domain = mono_jit_init("ORiginJITRuntime");
		OGN_CORE_ASSERT(root_domain, "[Script Engine] Mono Domain is NULL!");

		s_ScriptEngineData->RootDomain = root_domain;
		OGN_CORE_TRACE("[Script Engine] MONO Initialized");

		const char *mono_version = mono_get_runtime_build_info();
		OGN_CORE_TRACE("[Script Engine] MONO Version: {0}", mono_version);
	}

	void ScriptEngine::ShutdownMono()
	{
		OGN_PROFILER_LOGIC();

		if (!s_ScriptEngineData)
		{
			return;
		}

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_ScriptEngineData->AppDomain);
		s_ScriptEngineData->AppDomain = nullptr;
		s_ScriptEngineData->CoreAssembly = nullptr;

		mono_jit_cleanup(s_ScriptEngineData->RootDomain);
		s_ScriptEngineData->RootDomain = nullptr;

		OGN_CORE_TRACE("[Script Engine] Mono Shutdown");
	}

	void ScriptEngine::Init()
	{
		OGN_PROFILER_LOGIC();

		const auto app_assembly_path = Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;

		if (s_ScriptEngineData)
		{
			s_ScriptEngineData->AppAssemblyFilepath = app_assembly_path.generic_string();
			ReloadAssembly();
			return;
		}

		s_ScriptEngineData = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		// Script Core Assembly
		OGN_CORE_ASSERT(std::filesystem::exists("ScriptCore.dll"), "[Script Engine] Script core assembly not found!");
		LoadAssembly("ScriptCore.dll");
		LoadAppAssembly(app_assembly_path);
		LoadAssemblyClasses();

		// storing classes name into storage
		for (auto &it : s_ScriptEngineData->EntityClasses)
			s_ScriptEngineData->EntityScriptStorage.emplace_back(it.first);

		s_ScriptEngineData->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
		ScriptGlue::RegisterComponents();

		OGN_CORE_TRACE("[Script Engine] Initialized");
	}

	void ScriptEngine::Shutdown()
	{
		OGN_PROFILER_LOGIC();

		ShutdownMono();

		if (!s_ScriptEngineData)
		{
			return;
		}

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
		{
			return false;
		}

		s_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->CoreAssembly);
		return true;
	}

	static void OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event change_type)
	{
		OGN_PROFILER_LOGIC();

		if (!s_ScriptEngineData->AssemblyReloadingPending && change_type == filewatch::Event::modified)
		{
			s_ScriptEngineData->AssemblyReloadingPending = true;

			Application::Instance().SubmitToMainThread([]()
			{
				s_ScriptEngineData->AppAssemblyFilewatcher.reset();
				ScriptEngine::ReloadAssembly();
			});
		}
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path &filepath)
	{
		OGN_PROFILER_LOGIC();

		if (!exists(filepath))
		{
			const auto build_path = Project::GetActive()->GetProjectDirectory() / "build.bat";
			Utils::ExecuteScript(build_path.generic_string());
			OGN_CORE_ASSERT(exists(filepath), "[Script Engine] Failed to generate project file");
		}

		s_ScriptEngineData->AppAssemblyFilepath = filepath;
		s_ScriptEngineData->AppAssembly = Utils::LoadMonoAssembly(filepath);
		if (!s_ScriptEngineData->AppAssembly)
		{
			OGN_CORE_ASSERT(false, "[Script Engine] App Assembly is empty {}", filepath.generic_string());
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
			return s_ScriptEngineData->EntityClasses.contains(fullClassName);
		return false;		
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		OGN_PROFILER_LOGIC();

		if (const auto &sc = entity.GetComponent<ScriptComponent>(); EntityClassExists(sc.ClassName))
		{
			const UUID entity_id = entity.GetUUID();

			const auto instance = std::make_shared<ScriptInstance>(s_ScriptEngineData->EntityClasses[sc.ClassName], entity);
			s_ScriptEngineData->EntityInstances[entity_id] = instance;

			// Copy Fields Value from Editor to Runtime
			if (s_ScriptEngineData->EntityScriptFields.contains(entity_id))
			{
				ScriptFieldMap &fieldMap = s_ScriptEngineData->EntityScriptFields.at(entity_id);
				const auto classFields = instance->GetScriptClass()->GetFields();

				if (fieldMap.size() != classFields.size())
				{
                    // find any private members
                    std::vector<std::string> keysToRemove;
                    for (const auto &[fieldName, field] : fieldMap)
                    {
                        if (!classFields.contains(fieldName))
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
						const ScriptFieldType type = instance->GetScriptClass()->GetFields()[name].Type;
						fieldInstance.Field.Type = type;
						OGN_CORE_WARN("[Script Engine] Checking invalid type {0}", name);
						PUSH_CONSOLE_WARNING("[Script Engine] Checking invalid type {0}", name);
					}

					switch (fieldInstance.Field.Type)
					{
					case ScriptFieldType::Entity:
					{
						uint64_t uuid = *reinterpret_cast<uint64_t*>(fieldInstance.m_Buffer);
						if (uuid == 0)
						{
							OGN_CORE_ERROR("[Script Engine] Field '{}' (Entity class) is not assigned yet", name);
							PUSH_CONSOLE_ERROR("[Script Engine] Field '{}' (Entity class) is not assigned yet", name);
							continue;
						}

						MonoMethod *ctorMethod = s_ScriptEngineData->EntityClass.GetMethod(".ctor", 1);
						if (!ctorMethod)
						{
							OGN_CORE_ERROR("[Script Engine] Failed to find constructor {}", name);
							PUSH_CONSOLE_ERROR("[Script Engine] Failed to find constructor {}", name);
							continue;
						}

						// Create new instance
						MonoObject *entityInstance = ScriptEngine::InstantiateObject(s_ScriptEngineData->EntityClass.m_MonoClass);
						if (!entityInstance)
						{
							OGN_CORE_ERROR("[Script Engine] Failed to create Entity instance. {}", name);
							PUSH_CONSOLE_ERROR("[Script Engine] Failed to create Entity instance. {}", name);
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

		const UUID entity_id = entity.GetUUID();
		if (const auto &it = s_ScriptEngineData->EntityInstances.find(entity_id); it == s_ScriptEngineData->EntityInstances.end())
		{
			OGN_CORE_ERROR("[Script Engine] Entity script instance is not attached! {} {}", entity.GetTag(), entity_id);
			PUSH_CONSOLE_ERROR("[Script Engine] Entity script instance is not attached! {}", entity_id);
			return;
		}

		const auto instance = s_ScriptEngineData->EntityInstances.at(entity_id);
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
		if (!s_ScriptEngineData->EntityClasses.contains(name))
		{
			return nullptr;
		}

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

		const UUID entity_id = entity.GetUUID();
		return s_ScriptEngineData->EntityScriptFields[entity_id];
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
			OGN_CORE_ERROR("[Script Engine] Failed to find {}", uuid);
			PUSH_CONSOLE_ERROR("[Script Engine] Failed to find {} ", uuid);
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

		if (!s_ScriptEngineData->EntityInstances.contains(uuid))
		{
			PUSH_CONSOLE_ERROR("[Script Engine] Invalid Script Instance {}", uuid);
			OGN_CORE_ASSERT(false, "[Script Engine] Invalid Script Instance {}", uuid);
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

		const MonoTableInfo *typeDefinitionTable = mono_image_get_table_info(s_ScriptEngineData->AppAssemblyImage,
			MONO_TABLE_TYPEDEF);

		const i32 num_types = mono_table_info_get_rows(typeDefinitionTable);
		MonoClass *entity_class = mono_class_from_name(s_ScriptEngineData->CoreAssemblyImage,
			"ORiginEngine", "Entity");

		for (i32 i = 0; i < num_types; i++)
		{
			u32 cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

			const char *nameSpace = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char *className = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string full_name = className;

			if (strlen(nameSpace) != 0)
			{
				full_name = fmt::format("{}.{}", nameSpace, className);
			}

			MonoClass *mono_class = mono_class_from_name(s_ScriptEngineData->AppAssemblyImage, nameSpace, className);
			if (mono_class == entity_class)
			{
				continue;
			}

			if (const bool is_entity_master_class = mono_class_is_subclass_of(mono_class,
				entity_class, false); !is_entity_master_class)
			{
				continue;
			}

			const auto script_class = std::make_shared<ScriptClass>(nameSpace, className);
			s_ScriptEngineData->EntityClasses[full_name] = script_class;

			void *iterator = nullptr;

			while (MonoClassField *field = mono_class_get_fields(mono_class, &iterator))
			{
				const char *fieldName = mono_field_get_name(field);
				if (const uint32_t flags = mono_field_get_flags(field); flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType *type = mono_field_get_type(field);
					const ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					script_class->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}

		auto &entity_classes = s_ScriptEngineData->EntityClasses;
	}
}