// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"

namespace Origin
{

#define OGN_ADD_INTERNAL_CALLS(Name) mono_add_internal_call("ORiginEngine.InternalCalls::"#Name, Name)

	static void NativeLog(MonoString* string, int value)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);

		mono_free(cStr);
		OGN_CORE_WARN("{} {}", str, value);
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		return glm::dot(*parameter, *parameter);
	}

	void ScriptGlue::RegisterFunctions()
	{
		OGN_ADD_INTERNAL_CALLS(NativeLog);
		OGN_ADD_INTERNAL_CALLS(NativeLog_Vector);
		OGN_ADD_INTERNAL_CALLS(NativeLog_VectorDot);
	}

}