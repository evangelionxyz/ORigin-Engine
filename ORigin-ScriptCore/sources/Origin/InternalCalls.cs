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

        // Script Instance
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);

        // Transform
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityID, ref Vector3 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector3 value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

        // TextComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string TextComponent_GetText(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetText(ulong entityID, string textString);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_GetColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetColor(ulong entityID, ref Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_GetKerning(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetKerning(ulong entityID, float kerning);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_GetLineSpacing(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetLineSpacing(ulong entityID, float lineSpacing);

        // Circle Renderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetColor(ulong entityID, out Vector4 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetColor(ulong entityID, ref Vector4 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetThickness(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetThickness(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetFade(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetFade(ulong entityID, ref float value);

        // Box Collider Renderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetOffset(ulong entityID, out Vector2 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetOffset(ulong entityID, ref Vector2 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetSize(ulong entityID, out Vector2 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetSize(ulong entityID, ref Vector2 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetDensity(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetDensity(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetFriction(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetFriction(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetRestitution(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetRestitution(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetRestitutionThreshold(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetRestitutionThreshold(ulong entityID, ref float value);

        // Circle Collider Renderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetOffset(ulong entityID, out Vector2 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetOffset(ulong entityID, ref Vector2 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetRadius(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRadius(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetDensity(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetDensity(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetFriction(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetFriction(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetRestitution(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRestitution(ulong entityID, ref float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetRestitutionThreshold(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRestitutionThreshold(ulong entityID, ref float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRenderer2DComponent_GetColor(ulong entityID, out Vector4 result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRenderer2DComponent_SetColor(ulong entityID, ref Vector4 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRenderer2DComponent_GetTilingFactor(ulong entityID, out float result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRenderer2DComponent_SetTilingFactor(ulong entityID, ref float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode keycode);
    }
}