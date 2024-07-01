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
    internal extern static void Entity_AddComponent(ulong entityID, Type componentType);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ulong Entity_FindEntityByName(string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static ulong Entity_Instantiate(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_Destroy(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_SetVisibility(ulong entityID, bool value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_GetVisibility(ulong entityID, out bool result);

    // Script Instance
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static object GetScriptInstance(ulong entityID);

    // Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_SetTranslation(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_SetRotation(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformComponent_SetScale(ulong entityID, Vector3 value);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void RigidbodyComponent_SetVelocity(ulong entityID, Vector3 value, bool autoWake);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector2 RigidbodyComponent_GetVelocity(ulong entityID, out Vector3 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void RigidbodyComponent_SetForce(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void RigidbodyComponent_SetVelocityForce(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void RigidbodyComponent_SetImpulseForce(ulong entityID, Vector3 value);

    // Rigidbody 2D
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector2 Rigidbody2DComponent_GetVelocity(ulong entityID, out Vector2 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rigidbody2DComponent_SetVelocity(ulong entityID, Vector2 value);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, Vector2 impulse, Vector2 point, bool wake);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, Vector2 impulse, bool wake);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rigidbody2DComponent_ApplyForce(ulong entityID, Vector2 force, Vector2 point, bool wake);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Rigidbody2DComponent_ApplyForceToCenter(ulong entityID, Vector2 force, bool wake);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Rigidbody2DComponent_IsContactWithTag(ulong entityID, string contactWith);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string Rigidbody2DComponent_GetContactWithTag(ulong entityID);

    // Audio Component
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetName(ulong entityID, string nameString);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string AudioComponent_GetName(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetPitch(ulong entityID, float pitch);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float AudioComponent_GetPitch(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetVolume(ulong entityID, float volume);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float AudioComponent_GetVolume(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetMinDistance(ulong entityID, float minDistance);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float AudioComponent_GetMinDistance(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetMaxDistance(ulong entityID, float maxDistance);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float AudioComponent_GetMaxDistance(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetLooping(ulong entityID, bool looping);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool AudioComponent_IsLooping(ulong entityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetPlayAtStart(ulong entityID, bool playAtStart);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool AudioComponent_IsPlayAtStart(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_SetSpatial(ulong entityID, bool spatial);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool AudioComponent_IsSpatial(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_Play(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void AudioComponent_Stop(ulong entityID);

    // TextComponent
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string TextComponent_GetText(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TextComponent_SetText(ulong entityID, string textString);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TextComponent_GetColor(ulong entityID, out Vector3 color);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TextComponent_SetColor(ulong entityID, Vector3 color);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float TextComponent_GetKerning(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TextComponent_SetKerning(ulong entityID, float kerning);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float TextComponent_GetLineSpacing(ulong entityID);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TextComponent_SetLineSpacing(ulong entityID, float lineSpacing);


    // UIComponent
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string UIComponent_TextComponent_GetText(ulong entityID, string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void UIComponent_TextComponent_SetText(ulong entityID, string name, string textString);


    // Circle Renderer
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_GetColor(ulong entityID, out Vector3 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_SetColor(ulong entityID, Vector3 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_GetThickness(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_SetThickness(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_GetFade(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleRendererComponent_SetFade(ulong entityID, float value);

    // Box Collider Renderer
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetOffset(ulong entityID, out Vector2 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetOffset(ulong entityID, Vector2 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetSize(ulong entityID, out Vector2 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetSize(ulong entityID, Vector2 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetDensity(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetDensity(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetFriction(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetFriction(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetRestitution(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetRestitution(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_GetRestitutionThreshold(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void BoxCollider2DComponent_SetRestitutionThreshold(ulong entityID, float value);

    // Circle Collider Renderer
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetOffset(ulong entityID, out Vector2 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetOffset(ulong entityID, Vector2 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetRadius(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetRadius(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetDensity(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetDensity(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetFriction(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetFriction(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetRestitution(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetRestitution(ulong entityID, float value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_GetRestitutionThreshold(ulong entityID, out float result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void CircleCollider2DComponent_SetRestitutionThreshold(ulong entityID, float value);

    // SpriteRenderer2DComponent
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_GetFlipX(ulong entityID, out bool flip);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_SetFlipX(ulong entityID, bool flip);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_GetFlipY(ulong entityID, out bool flip);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_SetFlipY(ulong entityID, bool flip);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_GetColor(ulong entityID, out Vector4 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_SetColor(ulong entityID, Vector4 value);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_GetTilingFactor(ulong entityID, out Vector2 result);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteRenderer2DComponent_SetTilingFactor(ulong entityID, Vector2 value);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteAnimationComponent_GetActiveState(ulong entityID, out string stateString);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SpriteAnimationComponent_SetActiveState(ulong entityID, string stateString);


    // INPUT
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Input_IsKeyPressed(KeyCode keycode);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Input_IsKeyReleased(KeyCode keycode);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Input_IsMouseDown(MouseCode code);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Input_IsMouseUp(MouseCode code);
  }
}