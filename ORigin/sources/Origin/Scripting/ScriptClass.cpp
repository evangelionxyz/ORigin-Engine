#include "pch.h"
#include "ScriptClass.h"

#include <mono/jit/jit.h>

#include "ScriptEngine.h"

namespace origin
{
    ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className, bool core)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(core ? ScriptEngine::GetCoreAssemblyImage() : ScriptEngine::GetAppAssemblyImage(), classNamespace.c_str(), className.c_str());
    }

    MonoObject *ScriptClass::Instantiate()
    {
        return ScriptEngine::InstantiateObject(m_MonoClass);
    }

    MonoMethod *ScriptClass::GetMethod(const std::string &name, int parameterCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject *ScriptClass::InvokeMethod(MonoObject *classInstance, MonoMethod *method, void **params)
    {
        // If break here, check you C# code, the members should not a null object
        MonoObject *exception = nullptr;
        MonoObject *result = mono_runtime_invoke(method, classInstance, params, &exception);

        if (exception)
        {
            HandleException(exception);
            return nullptr;
        }

        return result;
    }

    void ScriptClass::HandleException(MonoObject *exception)
    {
        MonoClass *exceptionClass = mono_object_get_class(exception);
        MonoProperty *messageProperty = mono_class_get_property_from_name(exceptionClass, "Message");
        MonoMethod *getMessageMethod = mono_property_get_get_method(messageProperty);
        MonoString *messageString = (MonoString *)mono_runtime_invoke(getMessageMethod, exception, nullptr, nullptr);

        const char *message = mono_string_to_utf8(messageString);

        // Get stack trace
        MonoProperty *stackTraceProperty = mono_class_get_property_from_name(exceptionClass, "StackTrace");
        MonoMethod *getStackTraceMethod = mono_property_get_get_method(stackTraceProperty);
        MonoString *stackTraceString = (MonoString *)mono_runtime_invoke(getStackTraceMethod, exception, nullptr, nullptr);

        const char *stackTrace = mono_string_to_utf8(stackTraceString);
        OGN_CORE_ERROR("[ScriptClass] {0}", message);
        OGN_CORE_ERROR("[ScriptClass] {0}", stackTrace);

        mono_free((void *)message);
        mono_free((void *)stackTrace);
    }
}
