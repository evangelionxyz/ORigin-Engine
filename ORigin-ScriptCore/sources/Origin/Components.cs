// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
    public struct TransformComponent
    {
        Vector3 Translation;
        Vector3 Rotation;
        Vector3 Scale;

        public TransformComponent(Vector3 translation, Vector3 rotation, Vector3 scale)
        {
            Translation = translation;
            Rotation = rotation;
            Scale = scale;
        }

        public TransformComponent(TransformComponent other)
        {
            this = other;
        }
    }

    public class Components
    {
    }
}
