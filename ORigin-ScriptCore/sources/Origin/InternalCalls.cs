// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using System.Runtime.CompilerServices;

namespace ORiginEngine
{
    public static class InternalCalls
    {
        // Entity Method
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_FindEntityByName(string name);

        // Component Method
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode keycode);
    }
}