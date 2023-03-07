// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"
#include "Origin\Scene\Component\Component.h"

#include "mono\jit/jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\object.h"

namespace Origin
{
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

		// Runtime
		Scene* SceneContext = nullptr;
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, std::shared_ptr<ScriptInstance>> EntityInstances;
	};

	ScriptEngineData* s_Data = nullptr;

	// Scrip Engine
	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("ORiginJITRuntime");
		OGN_CORE_ASSERT(rootDomain, "Mono Domain is NULL!");

		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// cleanup
		mono_assembly_close(s_Data->CoreAssembly);
		s_Data->AppDomain = nullptr;
		s_Data->RootDomain = nullptr;
	}

	void ScriptEngine::Init()
	{
		// ========================
		// Initializing Script Engine
		// ========================

		s_Data = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		bool status = LoadAssembly("resources/scripts/ORigin-ScriptCore.dll");
		if (!status)
		{
			OGN_CORE_ERROR("[ScriptEngine] Could not load ORigin-ScriptCore assembly.");
			return;
		}

		status = LoadAppAssembly("SandboxProject/Binaries/Sandbox.dll");
		if (!status)
		{
			OGN_CORE_ERROR("[ScriptEngine] Could not load Sandbox assembly.");
			return;
		}

		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();
		s_Data->EntityClass = ScriptClass("ORiginEngine", "Entity", true);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		s_Data->EntityClasses.clear();
		s_Data->EntityInstances.clear();

		// Cleanup memory before shutdown
		delete s_Data;
		OGN_CORE_WARN("Script Engine Shutdown");
	}

	bool ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("ORiginScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		if (s_Data->CoreAssembly == nullptr)
			return false;

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		return true;
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
		if (s_Data->AppAssembly == nullptr)
			return false;

		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		return true;
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
		s_Data->EntityInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		auto& sc = entity.GetComponent<ScriptComponent>();
		if (EntityClassExists(sc.ClassName))
		{
			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entity.GetUUID()] = instance;
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float time)
	{
		UUID& entityID = entity.GetUUID();

		if (s_Data->EntityInstances.find(entityID) == s_Data->EntityInstances.end())
		{
			OGN_CORE_WARN("Entity Script Instance not found!");
			return;
		}

		std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances.at(entityID);
		instance->InvokeOnUpdate(time);
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionTable);

		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "ORiginEngine", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);

			if(monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_Data->EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);

			OGN_CORE_INFO("{}.{}", nameSpace, name);
		}
	}

	// Script Class
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool core)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(core ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
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

		m_OnConstructor = s_Data->EntityClass.GetMethod(".ctor", 1);
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
}