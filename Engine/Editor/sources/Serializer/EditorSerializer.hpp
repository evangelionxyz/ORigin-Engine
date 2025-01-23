// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef EDITOR_SERIALIZER_HPP
#define EDITOR_SERIALIZER_HPP

#include <filesystem>

namespace origin
{
    class EditorLayer;
    class EditorSerializer
    {
    public:
        static bool Serialize(EditorLayer *editor, const std::filesystem::path &filepath);
        static bool Deserialize(EditorLayer *editor, const std::filesystem::path &filepath);
    };
}

#endif