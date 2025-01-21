#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Origin/Core/Types.h>

#define MAX_NUM_BONES_PER_VERTEX (4)

struct VertexBoneData
{
    u32 BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

    VertexBoneData() = default;

    void AddBoneData(u32 bone_id, f32 weight)
    {
        for (u32 i = 0; i < std::size(BoneIDs); ++i)
        {
            if (Weights[i] == 0.0f)
            {
                BoneIDs[i] = bone_id;
                Weights[i] = weight;
                printf("bone %d weight %f index %i\n", bone_id, weight, i);
                return;
            }
        }
    }
};

std::vector<VertexBoneData> vertex_to_bones;
std::vector<i32> mesh_base_vertex;
std::map<std::string, u32> bone_name_to_index_map;

i32 get_bone_id(const aiBone *bone)
{
    i32 bone_id = 0;
    std::string bone_name(bone->mName.data);

    if (!bone_name_to_index_map.contains(bone_name))
    {
        bone_id = static_cast<i32>(bone_name_to_index_map.size());
        bone_name_to_index_map[bone_name] = bone_id;
    }
    else
    {
        bone_id = bone_name_to_index_map[bone_name];
    }

    return bone_id;
}

void parse_single_bone(int mesh_index, const aiBone *bone)
{
    printf("    Bone '%s' num vertices affected by this bone: %d\n", bone->mName.C_Str(), bone->mNumWeights);

    i32 bone_id = get_bone_id(bone);
    for (u32 i = 0; i < bone->mNumWeights; ++i)
    {
        if (i == 0)
            printf("\n");
        const aiVertexWeight &vw = bone->mWeights[i];

        u32 global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
        // printf("Vertex id %d ", global_vertex_id);

        vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
    }

    printf("\n");
}

void parse_mesh_bones(i32 mesh_index, const aiMesh *mesh)
{
    for (i32 i = 0; i < mesh->mNumBones; ++i)
        parse_single_bone(mesh_index, mesh->mBones[i]);
}

void parse_meshes(const aiScene *scene)
{
    printf("====================================\n");
    printf("Parsing %d meshes\n\n", scene->mNumMeshes);

    i32 total_vertices = 0;
    i32 total_indices = 0;
    i32 total_bones = 0;

    mesh_base_vertex.resize(scene->mNumMeshes);

    for (i32 mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
    {
        const aiMesh *mesh = scene->mMeshes[mesh_index];

        i32 num_vertices = mesh->mNumVertices;
        i32 num_indices = mesh->mNumFaces * 3;
        i32 num_bones = mesh->mNumBones;

        mesh_base_vertex[mesh_index] = total_vertices;

        printf("  Mesh %d '%s': vertices %d indices %d bones %d\n\n", mesh_index, mesh->mName.C_Str(), num_vertices, num_indices, num_bones);

        total_vertices += num_vertices;
        total_indices += num_indices;
        total_bones += num_bones;

        vertex_to_bones.resize(total_vertices);

        if (mesh->HasBones())
        {
            parse_mesh_bones(mesh_index, mesh);
        }

        printf("\n");
    }

    printf("Total vertices %d indices %d bones %d\n", total_vertices, total_indices, total_bones);
}

void parse_scene(const aiScene *scene)
{
    parse_meshes(scene);
}