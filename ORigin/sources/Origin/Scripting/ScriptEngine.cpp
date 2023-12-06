// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"
#include "Origin\Scene\Components.h"
#include "Origin\Project\Project.h"

#include "Origin\Core\Application.h"

#include "mono\jit\jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\object.h"
#include "mono\metadata\tabledefs.h"
#include "mono\metadata\mono-debug.h"

#include <FileWatch.hpp>

namespace origin
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{"System.Boolean", ScriptFieldType::Bool},
		{"System.Single", ScriptFieldType::Float},

		{"System.Char",		ScriptFieldType::Char},
		{"System.Byte",		ScriptFieldType::Byte},
		{"System.Double", ScriptFieldType::Double},

		{"System.Int16",	ScriptFieldType::Short},
		{"System.Int32",	ScriptFieldType::Int},
		{"System.Int64",	ScriptFieldType::Long},

		{"System.UInt16", ScriptFieldType::UShort},
		{"System.UInt32", ScriptFieldType::UInt},
		{"System.UInt64", ScriptFieldType::ULong},
		{"System.UInt",		ScriptFieldType::UByte},

		{"ORiginEngine.Vector2",ScriptFieldType::Vector2},
		{"ORiginEngine.Vector3",ScriptFieldType::Vector3},
		{"ORiginEngine.Vector4",ScriptFieldType::Vector4},
		{"ORiginEngine.Entity",	ScriptFieldType::Entity},
	};

	namespace Utils
	{
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
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
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				OGN_CORE_INFO("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				OGN_CORE_ERROR("Unkown Field Type : {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		ScriptClass EntityClass;
		std::vector<std::string> EntityScriptStorage;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFilewatcher;
		bool AssemblyReloadingPending = false;

		Scene* SceneContext = nullptr;
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, std::shared_ptr<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;
	};

	ScriptEngineData* s_ScriptEngineData = nullptr;

	// Scrip Engine
	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("ORiginJITRuntime");
		OGN_CORE_ASSERT(rootDomain, "Mono Domain is NULL!")

		s_ScriptEngineData->RootDomain = rootDomain;
		OGN_CORE_INFO("MONO: Initialized");
	}

	void ScriptEngine::ShutdownMono()
	{
		if (s_ScriptEngineData)
		{
			mono_domain_set(mono_get_root_domain(), false);
			if (s_ScriptEngineData->RootDomain)
			{
				mono_domain_unload(s_ScriptEngineData->AppDomain);
				s_ScriptEngineData->AppDomain = nullptr;
				s_ScriptEngineData->CoreAssembly = nullptr;

				mono_jit_cleanup(s_ScriptEngineData->RootDomain);
				s_ScriptEngineData->RootDomain = nullptr;
			}

			OGN_CORE_INFO("MONO: Shutdown");
		}
	}

	void ScriptEngine::Init()
	{
		s_ScriptEngineData = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();
		
		// Script Core Assembly
		LoadAssembly("Resources/ScriptCore/ORigin-ScriptCore.dll");

		auto appAssemblyPath = Project::GetActiveProjectDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;
		LoadAppAssembly(appAssemblyPath);

		LoadAssemblyClasses();

		// storing classes name into storage
		for (auto& it : s_ScriptEngineData->EntityClasses)
			s_ScriptEngineData->EntityScriptStorage.emplace_back(it.first);

		s_ScriptEngineData->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
		ScriptGlue::RegisterComponents();

		OGN_CORE_INFO("SCRIPT ENGINE: Initialized");
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		s_ScriptEngineData->EntityClasses.clear();
		s_ScriptEngineData->EntityInstances.clear();

		delete s_ScriptEngineData;

		OGN_CORE_WARN("SCRIPT ENGINE: Shutdown");
	}

	bool ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_ScriptEngineData->AppDomain = mono_domain_create_appdomain("ORiginScriptRuntime", nullptr);
		mono_domain_set(s_ScriptEngineData->AppDomain, true);

		s_ScriptEngineData->CoreAssemblyFilepath = filepath;

		s_ScriptEngineData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		if (s_ScriptEngineData->CoreAssembly == nullptr)
			return false;

		s_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->CoreAssembly);
		return true;
	}

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
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

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_ScriptEngineData->AppAssemblyFilepath = filepath;

		s_ScriptEngineData->AppAssembly = Utils::LoadMonoAssembly(filepath);
		if (s_ScriptEngineData->AppAssembly == nullptr)
			return false;

		s_ScriptEngineData->AppAssemblyImage = mono_assembly_get_image(s_ScriptEngineData->AppAssembly);
		s_ScriptEngineData->AppAssemblyFilewatcher = std::make_unique<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_ScriptEngineData->AssemblyReloadingPending = false;

		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_ScriptEngineData->AppDomain);

		LoadAssembly(s_ScriptEngineData->CoreAssemblyFilepath);
		LoadAppAssembly(s_ScriptEngineData->AppAssemblyFilepath);

		LoadAssemblyClasses();

		// storing classes name into storage
		s_ScriptEngineData->EntityScriptStorage.clear();

		for (auto& it : s_ScriptEngineData->EntityClasses)
			s_ScriptEngineData->EntityScriptStorage.emplace_back(it.first);

		// retrieve entity class
		s_ScriptEngineData->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
		ScriptGlue::RegisterComponents();
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_ScriptEngineData->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_ScriptEngineData->SceneContext = nullptr;
		s_ScriptEngineData->EntityInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_ScriptEngineData->EntityClasses.find(fullClassName) != s_ScriptEngineData->EntityClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		auto& sc = entity.GetComponent<ScriptComponent>();
		if (EntityClassExists(sc.ClassName))
		{
			UUID entityID = entity.GetUUID();

			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_ScriptEngineData->EntityClasses[sc.ClassName], entity);
			s_ScriptEngineData->EntityInstances[entityID] = instance;

			// Copy Fields Value from Editor to Runtime
			if (s_ScriptEngineData->EntityScriptFields.find(entityID) != s_ScriptEngineData->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_ScriptEngineData->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			// C# On Create Function
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float time)
	{
		UUID& entityID = entity.GetUUID();

		auto& it = s_ScriptEngineData->EntityInstances.find(entityID);
		if (it == s_ScriptEngineData->EntityInstances.end())
		{
			OGN_CORE_WARN("Entity Script Instance not found!");
			return;
		}

		std::shared_ptr<ScriptInstance> instance = s_ScriptEngineData->EntityInstances.at(entityID);
		instance->InvokeOnUpdate(time);
	}

	MonoString* ScriptEngine::CreateString(const char* string)
	{
		return mono_string_new(s_ScriptEngineData->AppDomain, string);
	}

	std::shared_ptr<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_ScriptEngineData->EntityClasses.find(name) == s_ScriptEngineData->EntityClasses.end())
			return nullptr;

		return s_ScriptEngineData->EntityClasses.at(name);
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_ScriptEngineData->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		OGN_CORE_ASSERT(entity, "ScriptEngine::GetScriptFieldMap: Failed to get entity");

		UUID entityID = entity.GetUUID();
		return s_ScriptEngineData->EntityScriptFields[entityID];
	}

	std::vector<std::string> ScriptEngine::GetScriptClassStorage()
	{
		return s_ScriptEngineData->EntityScriptStorage;
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID uuid)
	{
		auto& it = s_ScriptEngineData->EntityInstances.find(uuid);
		if (it == s_ScriptEngineData->EntityInstances.end())
		{
			OGN_CORE_ERROR("Script Instance: Failed to find {} ", uuid);
			return nullptr;
		}

		return it->second;
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_ScriptEngineData->SceneContext;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_ScriptEngineData->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
	{
		OGN_CORE_ASSERT(s_ScriptEngineData->EntityInstances.find(uuid) != s_ScriptEngineData->EntityInstances.end(),
			"Invalid Script Instance");

		return s_ScriptEngineData->EntityInstances.at(uuid)->GetMonoObject();
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_ScriptEngineData->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_ScriptEngineData->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionTable = mono_image_get_table_info(s_ScriptEngineData->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionTable);

		MonoClass* entityClass = mono_class_from_name(s_ScriptEngineData->CoreAssemblyImage, "ORiginEngine", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_ScriptEngineData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(s_ScriptEngineData->AppAssemblyImage, nameSpace, className);

			if(monoClass == entityClass)
				continue;

			const bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			std::shared_ptr<ScriptClass> scriptClass = std::make_shared<ScriptClass>(nameSpace, className);

			s_ScriptEngineData->EntityClasses[fullName] = scriptClass;

			int fieldCount = mono_class_num_fields(monoClass);

			//OGN_CORE_WARN("{} has {} fields: ", className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					//OGN_CORE_WARN("{} ({}) is public", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->m_Fields[fieldName] = {fieldType, fieldName, field};
				}
			}
		}

		auto& entityClasses = s_ScriptEngineData->EntityClasses;
	}

	// Script Class
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool core)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(core ? s_ScriptEngineData->CoreAssemblyImage : s_ScriptEngineData->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_OnConstructor = s_ScriptEngineData->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate");
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Entity Constructor
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnConstructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if(m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float time)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &time;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);

		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);

		return true;
	}
}