#include "pch.h"
#include "ScriptInstance.h"
#include "ScriptEngine.h"
#include "Origin/Scene/Entity.h"
#include "ScriptClass.h"
#include "Origin/Core/ConsoleManager.h"

#include <mono/jit/jit.h>

namespace origin
{
    ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        OGN_PROFILER_LOGIC();

        m_Instance = scriptClass->Instantiate();

        m_OnConstructor = ScriptEngine::GetEntityClass()->GetMethod(".ctor", 1);
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate");
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

        // Entity Constructor
        {
            UUID entityID = entity.GetUUID();
            void *param = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_OnConstructor, &param);
        }
    }

    void ScriptInstance::InvokeOnCreate()
    {
        OGN_PROFILER_LOGIC();

        if (m_OnCreateMethod)
        {
            m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
        }
    }

    void ScriptInstance::InvokeOnUpdate(float time)
    {
        OGN_PROFILER_LOGIC();

        if (m_OnUpdateMethod)
        {
            void *param = &time;
            m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
        }
    }

    bool ScriptInstance::GetFieldValueInternal(const std::string &name, void *buffer)
    {
        OGN_PROFILER_LOGIC();

        const auto &fields = m_ScriptClass->GetFields();

        auto it = fields.find(name);
        if (it == fields.end())
        {
            PUSH_CONSOLE_ERROR("[Script Instance] Failed to Get Internal Value");
            OGN_CORE_ERROR("[Script Instance] Failed to Get Internal Value");
            return false;
        }

        const ScriptField &field = it->second;
        if (field.Type == ScriptFieldType::Entity)
        {
            // Get Entity Field from App Class
            MonoObject *fieldValue = nullptr;
            mono_field_get_value(m_Instance, field.ClassField, &fieldValue);
            if (!fieldValue || !fieldValue->vtable)
            {
                PUSH_CONSOLE_ERROR("[Script Instance] Could not get field '{0}' in class", name);
                OGN_CORE_ERROR("[Script Instance] Could not get field '{0}' in class", name);
                return false;
            }

            // get the ID from field's class
            MonoClass *klass = mono_object_get_class(fieldValue);
            MonoClassField *idField = mono_class_get_field_from_name(klass, "ID");
            if (!idField)
            {
                PUSH_CONSOLE_ERROR("[Script Instance] Failed to find field '{0}' in Entity class", name);
                OGN_CORE_ERROR("[Script Instance] Failed to find field '{0}' in Entity class", name);
                return false;
            }

            mono_field_get_value(fieldValue, idField, buffer);
        }
        else
        {
            mono_field_get_value(m_Instance, field.ClassField, buffer);
        }

        return true;
    }

    bool ScriptInstance::SetFieldValueInternal(const std::string &name, const void *value)
    {
        OGN_PROFILER_LOGIC();

        const auto &fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
        {
            PUSH_CONSOLE_ERROR("[Script Instance] Failed to set field '{0}' value", name);
            OGN_CORE_ERROR("[Script Instance] Failed to set field '{0}' value", name);
            return false;
        }

        const ScriptField &field = it->second;

        if (field.Type == ScriptFieldType::Entity)
        {
            MonoObject *object = static_cast<MonoObject *>(const_cast<void *>(value));
            mono_field_set_value(m_Instance, field.ClassField, object);
        }
        else
        {
            mono_field_set_value(m_Instance, field.ClassField, (void *)value);
        }

        return true;
    }
}
