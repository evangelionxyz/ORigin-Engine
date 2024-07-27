// Copyright (c) Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class SpriteAnimationComponent : Component
    {
        public string ActiveState
        {
            get
            {
                InternalCalls.SpriteAnimationComponent_GetActiveState(Entity.ID, out string result);
                return result;
            }
            set
            {
                InternalCalls.SpriteAnimationComponent_SetActiveState(Entity.ID, value);
            }
        }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.ID, value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, value);
            }
        }
    }

    public class Rigidbody2DComponent : Component
    {
        public Vector2 Velocity
        {
            get
            {
                InternalCalls.Rigidbody2DComponent_GetVelocity(Entity.ID, out Vector2 result);
                return result;
            }
            set
            {
                InternalCalls.Rigidbody2DComponent_SetVelocity(Entity.ID, value);
            }
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, impulse, worldPosition, wake);
        }

        public void ApplyLinearImpulseToCenter(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, impulse, wake);
        }

        public void ApplyForce(Vector2 force, Vector2 point, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyForce(Entity.ID, force, point, wake);
        }

        public void ApplyForceToCenter(Vector2 force, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyForceToCenter(Entity.ID, force, wake);
        }

        public bool IsContactWithTag(string objectTag)
        {
            return InternalCalls.Rigidbody2DComponent_IsContactWithTag(Entity.ID, objectTag);
        }

        public string GetContactWithTag()
        {
            return InternalCalls.Rigidbody2DComponent_GetContactWithTag(Entity.ID);
        }
    }

    public class AudioComponent : Component
    {
        public void Play()
        {
            InternalCalls.AudioComponent_Play(Entity.ID);
        }
        public void Stop()
        {
            InternalCalls.AudioComponent_Stop(Entity.ID);
        }

        public string Name
        {
            get => InternalCalls.AudioComponent_GetName(Entity.ID);
            set => InternalCalls.AudioComponent_SetName(Entity.ID, value);
        }
        public float Volume
        {
            get => InternalCalls.AudioComponent_GetVolume(Entity.ID);
            set => InternalCalls.AudioComponent_SetVolume(Entity.ID, value);
        }
        public float MinDistance
        {
            get => InternalCalls.AudioComponent_GetMinDistance(Entity.ID);
            set => InternalCalls.AudioComponent_SetMinDistance(Entity.ID, value);
        }

        public float MaxDistance
        {
            get => InternalCalls.AudioComponent_GetMaxDistance(Entity.ID);
            set => InternalCalls.AudioComponent_SetMaxDistance(Entity.ID, value);
        }
        public bool Looping
        {
            get => InternalCalls.AudioComponent_IsLooping(Entity.ID);
            set => InternalCalls.AudioComponent_SetLooping(Entity.ID, value);
        }
        public bool Spatial
        {
            get => InternalCalls.AudioComponent_IsSpatial(Entity.ID);
            set => InternalCalls.AudioComponent_SetSpatial(Entity.ID, value);
        }
        public bool PlayAtStart
        {
            get => InternalCalls.AudioComponent_IsPlayAtStart(Entity.ID);
            set => InternalCalls.AudioComponent_SetPlayAtStart(Entity.ID, value);
        }

    }

    public class TextComponent : Component
    {
        public string Text
        {
            get => InternalCalls.TextComponent_GetText(Entity.ID);
            set => InternalCalls.TextComponent_SetText(Entity.ID, value);
        }

        public Vector3 Color
        {
            get
            {
                InternalCalls.TextComponent_GetColor(Entity.ID, out Vector3 color);
                return color;
            }
            set
            {
                InternalCalls.TextComponent_SetColor(Entity.ID, value);
            }
        }

        public float Kerning
        {
            get => InternalCalls.TextComponent_GetKerning(Entity.ID);
            set => InternalCalls.TextComponent_SetKerning(Entity.ID, value);
        }

        public float LineSpacing
        {
            get => InternalCalls.TextComponent_GetLineSpacing(Entity.ID);
            set => InternalCalls.TextComponent_SetLineSpacing(Entity.ID, value);
        }
    }

    public class BoxCollider2DComponent : Component
    {
        public Vector2 Offset
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetOffset(Entity.ID, out Vector2 offset);
                return offset;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetOffset(Entity.ID, value);
            }
        }

        public Vector2 Size
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetSize(Entity.ID, out Vector2 size);
                return size;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetSize(Entity.ID, value);
            }
        }

        public float Density
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetDensity(Entity.ID, out float density);
                return density;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetDensity(Entity.ID, value);
            }
        }

        public float Friction
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetFriction(Entity.ID, out float friction);
                return friction;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetFriction(Entity.ID, value);
            }
        }

        public float Restitution
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetRestitution(Entity.ID, out float restitution);
                return restitution;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetRestitution(Entity.ID, value);
            }
        }

        public float RestitutionThreshold
        {
            get
            {
                InternalCalls.BoxCollider2DComponent_GetRestitutionThreshold(Entity.ID, out float restitutionThreshold);
                return restitutionThreshold;
            }
            set
            {
                InternalCalls.BoxCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
            }
        }
    }

    public class CircleCollider2DComponent : Component
    {
        public Vector2 Offset
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetOffset(Entity.ID, out Vector2 offset);
                return offset;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetOffset(Entity.ID, value);
            }
        }

        public float Radius
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetRadius(Entity.ID, out float radius);
                return radius;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetRadius(Entity.ID, value);
            }
        }

        public float Density
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetDensity(Entity.ID, out float density);
                return density;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetDensity(Entity.ID, value);
            }
        }

        public float Friction
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetFriction(Entity.ID, out float friction);
                return friction;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetFriction(Entity.ID, value);
            }
        }

        public float Restitution
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetRestitution(Entity.ID, out float restitution);
                return restitution;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetRestitution(Entity.ID, value);
            }
        }

        public float RestitutionThreshold
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetRestitutionThreshold(Entity.ID, out float restitutionThreshold);
                return restitutionThreshold;
            }
            set
            {
                InternalCalls.CircleCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
            }
        }
    }

    public class CircleRendererComponent : Component
    {
        public Vector3 Color
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetColor(Entity.ID, out Vector3 color);
                return color;
            }

            set
            {
                InternalCalls.CircleRendererComponent_SetColor(Entity.ID, value);
            }
        }

        public float Thickness
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetThickness(Entity.ID, out float thickness);
                return thickness;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetThickness(Entity.ID, value); ;
            }

        }

        public float Fade
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetFade(Entity.ID, out float fade);
                return fade;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetFade(Entity.ID, value);
            }
        }
    }

    public class SpriteRenderer2DComponent : Component
    {
        public Vector4 Color
        {
            get
            {
                InternalCalls.SpriteRenderer2DComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }
            set
            {
                InternalCalls.SpriteRenderer2DComponent_SetColor(Entity.ID, value);
            }

        }

        public bool FlipX
        {
            get
            {
                InternalCalls.SpriteRenderer2DComponent_GetFlipX(Entity.ID, out bool flip);
                return flip;
            }
            set
            {
                InternalCalls.SpriteRenderer2DComponent_SetFlipX(Entity.ID, value);
            }
        }

        public bool FlipY
        {
            get
            {
                InternalCalls.SpriteRenderer2DComponent_GetFlipY(Entity.ID, out bool flip);
                return flip;
            }
            set
            {
                InternalCalls.SpriteRenderer2DComponent_SetFlipY(Entity.ID, value);
            }
        }

        public Vector2 TilingFactor
        {
            get
            {
                InternalCalls.SpriteRenderer2DComponent_GetTilingFactor(Entity.ID, out Vector2 tilingfactor);
                return tilingfactor;
            }
            set
            {
                InternalCalls.SpriteRenderer2DComponent_SetTilingFactor(Entity.ID, value);
            }
        }
    }

    public class RigidbodyComponent : Component
    {
        public void AddForce(Vector3 force)
        {
            InternalCalls.RigidbodyComponent_AddForce(Entity.ID, force);
        }

        public void AddTorque(Vector3 torque)
        {
            InternalCalls.RigidbodyComponent_AddTorque(Entity.ID, torque);
        }

        public void AddForceAndTorque(Vector3 force, Vector3 torque)
        {
            InternalCalls.RigidbodyComponent_AddForceAndTorque(Entity.ID, force, torque);
        }

        public void AddImpulse(Vector3 impulse)
        {
            InternalCalls.RigidbodyComponent_AddImpulse(Entity.ID, impulse);
        }

        public void AddAngularImpulse(Vector3 impulse)
        {
            InternalCalls.RigidbodyComponent_AddAngularImpulse(Entity.ID, impulse);
        }

        public void AddLinearVelocity(Vector3 velocity)
        {
            InternalCalls.RigidbodyComponent_AddLinearVelocity(Entity.ID, velocity);
        }

        public void MoveKinematic(Vector3 position, Vector3 rotation, float deltaTime)
        {
            InternalCalls.RigidbodyComponent_MoveKinematice(Entity.ID, position, rotation, deltaTime);
        }

        public void SetPosition(Vector3 position, bool activate)
        {
            InternalCalls.RigidbodyComponent_SetPosition(Entity.ID, position, activate);
        }

        public void SetEulerAngleRotation(Vector3 rotation, bool activate)
        {
            InternalCalls.RigidbodyComponent_SetEulerAngleRotation(Entity.ID, rotation, activate);
        }

        public void SetLinearVelocity(Vector3 velocity)
        {
            InternalCalls.RigidbodyComponent_SetLinearVelocity(Entity.ID, velocity);
        }

        public float Friction
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetFriction(Entity.ID, out float value);
                return value;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetFriction(Entity.ID, value);
            }
        }

        public float Restitution
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetRestitution(Entity.ID, out float value);
                return value;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetRestitution(Entity.ID, value);
            }
            
        }

        public float GravityFactor
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetGravityFactor(Entity.ID, out float value);
                return value;
            }
            
            set
            {
                InternalCalls.RigidbodyComponent_SetGravityFactor(Entity.ID, value);
            }
        }

        public Vector3 GetCenterOfMassPosition()
        {
            InternalCalls.RigidbodyComponent_GetCenterOfMassPosition(Entity.ID, out Vector3 value);
            return value;
        }

        public Vector3 LinearVelocity
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetLinearVelocity(Entity.ID, out Vector3 value);
                return value;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetLinearVelocity(Entity.ID, value);
            }

        }

        public bool IsActive()
        {
            InternalCalls.RigidbodyComponent_IsActive(Entity.ID, out bool value);
            return value;
        }

        public void ActivateBody()
        {
            InternalCalls.RigidbodyComponent_ActivateBody(Entity.ID);
        }

        public void DeactivateBody()
        {
            InternalCalls.RigidbodyComponent_DeactivateBody(Entity.ID);
        }

        public void DestroyBody()
        {
            InternalCalls.RigidbodyComponent_DestroyBody(Entity.ID);
        }
    }

    public class UIComponent : Component
    {
        public string GetTextString(string name)
        {
            return InternalCalls.UIComponent_TextComponent_GetText(Entity.ID, name);
        }

        public void SetTextString(string name, string text)
        {
            InternalCalls.UIComponent_TextComponent_SetText(Entity.ID, name, text);
        }
    }
}
