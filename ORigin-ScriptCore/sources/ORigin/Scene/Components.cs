// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class AnimationComponent : Component
    {
        public string ActiveState
        {
            get
            {
                InternalCalls.AnimationComponent_GetActiveState(Entity.ID, out string result);
                return result;
            }
            set
            {
                InternalCalls.AnimationComponent_SetActiveState(Entity.ID, value);
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
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
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
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
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
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }
        }

    }

    public class Rigidbody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulseToCenter(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }

        public void ApplyForce(Vector2 force, Vector2 point, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyForce(Entity.ID, ref force, ref point, wake);
        }

        public void ApplyForceToCenter(Vector2 force, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyForceToCenter(Entity.ID, ref force, wake);
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

        public Vector4 Color
        {
            get
            {
                InternalCalls.TextComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }
            set
            {
                InternalCalls.TextComponent_SetColor(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetOffset(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetSize(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetDensity(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetFriction(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetRestitution(Entity.ID, ref value);
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
                InternalCalls.BoxCollider2DComponent_SetRestitutionThreshold(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetOffset(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetRadius(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetDensity(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetFriction(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetRestitution(Entity.ID, ref value);
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
                InternalCalls.CircleCollider2DComponent_SetRestitutionThreshold(Entity.ID, ref value);
            }
        }
    }

    public class CircleRendererComponent : Component
    {
        public Vector4 Color
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }

            set
            {
                InternalCalls.CircleRendererComponent_SetColor(Entity.ID, ref value);
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
                InternalCalls.CircleRendererComponent_SetThickness(Entity.ID, ref value); ;
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
                InternalCalls.CircleRendererComponent_SetFade(Entity.ID, ref value);
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
                InternalCalls.SpriteRenderer2DComponent_SetColor(Entity.ID, ref value);
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
                InternalCalls.SpriteRenderer2DComponent_SetTilingFactor(Entity.ID, ref value);
            }
        }
    }
}
