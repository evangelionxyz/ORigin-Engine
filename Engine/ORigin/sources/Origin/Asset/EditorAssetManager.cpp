// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"

#include "EditorAssetManager.h"
#include "AssetManager.h"
#include "Origin/Renderer/Material.h"
#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Core/ConsoleManager.h"
#include "AssetImporter.h"
#include "Origin/Project/Project.h"
#include <yaml-cpp/yaml.h>

namespace origin
{
    static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap =
    {
        { ".org", AssetType::Scene },
        { ".jpg", AssetType::Texture },
        { ".jpeg", AssetType::Texture },
        { ".png", AssetType::Texture },
        { ".ttf", AssetType::Font },
        { ".otf", AssetType::Font },
        { ".gltf", AssetType::Mesh },
        { ".glb", AssetType::Mesh },
        { ".obj", AssetType::Mesh },
        { ".dae", AssetType::Mesh },
        { ".mat", AssetType::Material },
        { ".ogg", AssetType::Audio },
        { ".mp3", AssetType::Audio },
        { ".wav", AssetType::Audio },
        { ".sprite", AssetType::SpritesSheet },
    };

    static AssetType GetAssetTypeFromFileExtension(const std::string_view extension)
    {
        if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
        {
            OGN_CORE_WARN("[Editor Asset Manager] Could not find AssetType for {}", extension);
            PUSH_CONSOLE_WARNING("Invalid asset type{}", extension);
            return AssetType::None;
        }

        return s_AssetExtensionMap.at(extension);
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const std::string_view &v)
    {
        out << std::string(v.data(), v.size());
        return out;
    }

    std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
    {
        if (!IsAssetHandleValid(handle))
            return nullptr;

        if (IsAssetLoaded(handle) && GetAssetType(handle) != AssetType::Scene)
        {
            return m_LoadedAssets.at(handle);
        }

        std::shared_ptr<Asset> asset;
        const AssetMetadata &metadata = GetMetadata(handle);

        switch (metadata.Type)
        {
        case AssetType::Font:
        {
            auto filepath = Project::GetActiveAssetDirectory() / metadata.Filepath;
            m_LoadedAssets[handle] = asset;
            FontImporter::LoadAsync(&m_LoadedAssets[handle], filepath);
            return nullptr;
            break;
        }
        default:
            asset = AssetImporter::ImportAsset(handle, metadata);
            break;
        }

        if (!asset)
        {
            OGN_CORE_ERROR("[Editor Asset Manager] Asset Import Failed! {}", metadata.Filepath.generic_string());
            PUSH_CONSOLE_ERROR("Failed to import asset! {}", metadata.Filepath.generic_string());
            return nullptr;
        }
        else
        {
            m_LoadedAssets[handle] = asset;
        }

        return asset;
    }

    bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        if (handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end())
            return true;

        return false;
    }

    bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }

    AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
    {
        if (!IsAssetHandleValid(handle))
            return AssetType::None;

        return m_AssetRegistry.at(handle).Type;
    }

    AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path &filepath)
    {
        AssetHandle handle;
        AssetMetadata metadata;

        metadata.Filepath = filepath;
        metadata.Type = GetAssetTypeFromFileExtension(filepath.extension().string());

        if (metadata.Type == AssetType::None)
        {
            OGN_CORE_ERROR("[Editor Asset Manager] Invalid Asset Type {}", filepath.generic_string());
            PUSH_CONSOLE_ERROR("Invalid asset type! {}", filepath.generic_string());
            return 0;
        }

        std::shared_ptr<Asset> asset;
        if (metadata.Type == AssetType::Font)
        {
            OGN_CORE_TRACE("{}", handle);
            m_LoadedAssets[handle] = asset;
            m_AssetRegistry[handle] = metadata;

            const auto asset_to_path = Project::GetActiveAssetDirectory() / metadata.Filepath;
            FontImporter::LoadAsync(&m_LoadedAssets[handle], asset_to_path);
            SerializeAssetRegistry();
            return handle;
        }
       

        asset = AssetImporter::ImportAsset(handle, metadata);

        if (asset)
        {
            asset->Handle = handle;
            m_LoadedAssets[handle] = asset;
            m_AssetRegistry[handle] = metadata;
            SerializeAssetRegistry();
            return handle;
        }

        return 0;
    }

    void EditorAssetManager::InsertAsset(AssetHandle handle, AssetMetadata metadata, std::function<std::shared_ptr<Asset>()> loader)
    {
        std::shared_ptr<Asset> asset;
        if (IsAssetLoaded(handle) && GetAssetType(handle) != AssetType::Scene)
        {
            asset = m_LoadedAssets.at(handle);
            return;
        }

        asset = loader();
        if (asset)
        {
            asset->Handle = handle;
            m_LoadedAssets[handle] = asset;
            m_AssetRegistry[handle] = metadata;

            SerializeAssetRegistry();
        }
    }

    void EditorAssetManager::RemoveAsset(AssetHandle handle)
    {
        if (m_AssetRegistry.find(handle) != m_AssetRegistry.end())
            m_AssetRegistry.erase(handle);

        if (m_LoadedAssets.find(handle) != m_LoadedAssets.end())
            m_LoadedAssets.erase(handle);

        SerializeAssetRegistry();
    }

    void EditorAssetManager::RemoveLoadedAsset(AssetHandle handle)
    {
        if (m_LoadedAssets.find(handle) != m_LoadedAssets.end())
            m_LoadedAssets.erase(handle);
    }

    const origin::AssetMetadata &EditorAssetManager::GetMetadata(AssetHandle handle) const
    {
        static AssetMetadata s_NullMetadata;
        auto it = m_AssetRegistry.find(handle);
        if (it == m_AssetRegistry.end())
            return s_NullMetadata;

        return it->second;
    }

    const std::filesystem::path &EditorAssetManager::GetFilepath(AssetHandle handle)
    {
        return GetMetadata(handle).Filepath;
    }

    void EditorAssetManager::SerializeAssetRegistry()
    {
        auto path = Project::GetActiveAssetRegistryPath();

        OGN_CORE_INFO("[Editor Asset Manager] Serialize Registry {}", path.string());
        PUSH_CONSOLE_INFO("Assets Saved!");

        for (const auto &[handle, metadata] : m_AssetRegistry)
        {
            if (metadata.Type == AssetType::Material)
            {
                std::filesystem::path filepath = Project::GetActiveAssetDirectory() / metadata.Filepath;
                std::shared_ptr<Material> mat = AssetManager::GetAsset<Material>(handle);
                MaterialSerializer::Serialize(filepath, mat);
            }
        }

        YAML::Emitter out;
        {
            out << YAML::BeginMap; // Root
            out << YAML::Key << "AssetRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto &[handle, metadata] : m_AssetRegistry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Handle" << YAML::Value << handle;
                out << YAML::Key << "Filepath" << YAML::Value << metadata.Filepath.generic_string();
                out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
                out << YAML::EndMap;
            }

            out << YAML::EndSeq;
            out << YAML::EndMap; // !Root
        }

        std::ofstream fout(path);
        fout << out.c_str();

        fout.close();
    }

    bool EditorAssetManager::DeserializeAssetRegistry()
    {
        auto path = Project::GetActiveAssetRegistryPath();

        if (!std::filesystem::exists(path))
        {
            PUSH_CONSOLE_ERROR("[Editor Asset Manager] Failed to deserialize AssetRegistry {}", path.generic_string());
            OGN_CORE_ASSERT(false, "[Editor Asset Manager] Failed to deserialize AssetRegistry");
            return false;
        }

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.generic_string());
        } catch (const YAML::ParserException e)
        {
            OGN_CORE_ASSERT(false, "[Editor Asset Manager] Failed to load project file: {}\n\t{}", path.generic_string(), e.what());
            return false;
        }

        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto &node : rootNode)
        {
            AssetHandle handle = node["Handle"].as<uint64_t>();
            auto &metadata = m_AssetRegistry[handle];
            metadata.Filepath = node["Filepath"].as<std::string>();
            metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
        }

        return true;
    }

}