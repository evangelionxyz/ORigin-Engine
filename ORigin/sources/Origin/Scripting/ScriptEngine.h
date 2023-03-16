// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Scene\Scene.h"
#include "Origin\Scene\Entity.h"

#include <filesystem>
#include <string>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Origin {

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool core = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount = 0);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		std::unordered_map<std::string, ScriptField> GetFields() { return m_Fields; }

	private:
		MonoClass* m_MonoClass = nullptr;
		std::string m_ClassName;
		std::string m_ClassNamespace;
		std::unordered_map<std::string, ScriptField> m_Fields;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float time);

		std::shared_ptr<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			SetFieldValueInternal(name, &value);
		}

	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);

	private:
		std::shared_ptr<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_OnConstructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[8];
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static bool LoadAssembly(const std::filesystem::path& filepath);
		static bool LoadAppAssembly(const std::filesystem::path& filepath);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);

		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float time);

		static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> GetEntityClasses();
		static std::vector<std::string> GetScriptClassStorage();
		static std::shared_ptr<ScriptInstance> GetEntityScriptInstance(UUID uuid);

		static Scene* GetSceneContext();
		static MonoImage* GetCoreAssemblyImage();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		friend class ScriptClass;
	};
}