// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
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
