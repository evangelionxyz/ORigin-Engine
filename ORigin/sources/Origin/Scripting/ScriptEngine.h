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

namespace Origin
{

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

	// script field + data storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}
		
		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 64, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 64, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}

	private:
		char m_Buffer[64];

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool core = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount = 0);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::unordered_map<std::string, ScriptField> GetFields() const { return m_Fields; }

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
			static_assert(sizeof(T) <= 8, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			static_assert(sizeof(T) <= 64, "Type too large!");
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

		inline static char s_FieldValueBuffer[64];
		friend class ScriptEngine;
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

		static std::shared_ptr<ScriptClass> GetEntityClass(const std::string& name);
		static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> GetEntityClasses();
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);

		static std::shared_ptr<ScriptInstance> GetEntityScriptInstance(UUID uuid);
		static std::vector<std::string> GetScriptClassStorage();

		static Scene* GetSceneContext();
		static MonoImage* GetCoreAssemblyImage();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		friend class ScriptClass;
	};

	namespace Utils
	{
		inline const char* ScriptFieldTypeToString(ScriptFieldType type)
		{
			switch (type)
			{
				case Origin::ScriptFieldType::None:		 return "None";
				case Origin::ScriptFieldType::Float:	 return "Float";
				case Origin::ScriptFieldType::Double:	 return "Double";
				case Origin::ScriptFieldType::Bool:		 return "Boolean";
				case Origin::ScriptFieldType::Char:		 return "Char";
				case Origin::ScriptFieldType::Byte:		 return "Byte";
				case Origin::ScriptFieldType::Short:	 return "Short";
				case Origin::ScriptFieldType::Int:		 return "Int";
				case Origin::ScriptFieldType::Long:		 return "Long";
				case Origin::ScriptFieldType::UByte:	 return "UByte";
				case Origin::ScriptFieldType::UShort:	 return "UShort";
				case Origin::ScriptFieldType::UInt:		 return "UInt";
				case Origin::ScriptFieldType::ULong:	 return "ULong";
				case Origin::ScriptFieldType::Vector2: return "Vec2";
				case Origin::ScriptFieldType::Vector3: return "Vec3";
				case Origin::ScriptFieldType::Vector4: return "Vec4";
				case Origin::ScriptFieldType::Entity:	 return "Entity";
			}

			OGN_CORE_ASSERT(false, "Invalid Script Field Type!");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view type)
		{
			if (type == "None")		return ScriptFieldType::None;
			if (type == "Float")	return ScriptFieldType::Float;
			if (type == "Double")	return ScriptFieldType::Double;
			if (type == "Boolean")return ScriptFieldType::Bool;
			if (type == "Char")		return ScriptFieldType::Char;
			if (type == "Byte")		return ScriptFieldType::Byte;
			if (type == "Short")	return ScriptFieldType::Short;
			if (type == "Int")		return ScriptFieldType::Int;
			if (type == "Long")		return ScriptFieldType::Long;
			if (type == "UByte")	return ScriptFieldType::UByte;
			if (type == "UShort")	return ScriptFieldType::UShort;
			if (type == "UInt")		return ScriptFieldType::UInt;
			if (type == "ULong")	return ScriptFieldType::ULong;
			if (type == "Vec2")		return ScriptFieldType::Vector2;
			if (type == "Vec3")		return ScriptFieldType::Vector3;
			if (type == "Vec4")		return ScriptFieldType::Vector4;
			if (type == "Entity")	return ScriptFieldType::Entity;
			return ScriptFieldType::None;
		}
	}
}