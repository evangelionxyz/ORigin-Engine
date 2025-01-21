#include "glad/glad.h"

#include "SM.hpp"

SkinnedMesh::~SkinnedMesh()
{
    Clear();
}

void SkinnedMesh::Clear()
{
    if (m_Buffers[0] != 0)
    {
        glDeleteBuffers(static_cast<GLsizei>(std::size(m_Buffers)), m_Buffers);
    }

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool SkinnedMesh::LoadMesh(const std::string &filename)
{
    Clear();

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    auto size = static_cast<GLsizei>(std::size(m_Buffers));
    glGenBuffers(size, m_Buffers);

    bool ret_val = false;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);
    if (scene)
    {
        ret_val = InitFromScene(scene, filename);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString());
    }

    glBindVertexArray(0);

    return ret_val;
}

void SkinnedMesh::Render()
{
    glBindVertexArray(m_VAO);

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Meshes[i].num_indices,
            GL_UNSIGNED_INT,
            (void *)(sizeof(u32) * m_Meshes[i].base_index),
            m_Meshes[i].base_vertex);
    }

    glBindVertexArray(0);
}

bool SkinnedMesh::InitFromScene(const aiScene *scene, const std::string &filename)
{
    m_Meshes.resize(scene->mNumMeshes);

    u32 num_vertices = 0;
    u32 num_indices = 0;

    CountVerticesAndIndices(scene, num_vertices, num_indices);
    ReserveSpace(num_vertices, num_indices);

    InitAllMeshes(scene);

    if (!InitMaterials(scene, filename))
    {
        return false;
    }

    PopulateBuffers();

    return true;
}

void SkinnedMesh::CountVerticesAndIndices(const aiScene *scene, u32 &num_vertices, u32 &num_indices)
{
    for (u32 i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].material_index = scene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].num_indices = scene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].base_vertex = num_vertices;
        m_Meshes[i].base_index = num_indices;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += m_Meshes[i].num_indices;
    }
}

void SkinnedMesh::ReserveSpace(u32 num_vertices, u32 num_indices)
{
    m_Positions.reserve(num_vertices);
    m_Normals.reserve(num_vertices);
    m_TexCoords.reserve(num_vertices);
    m_Bones.resize(num_vertices);
}

void SkinnedMesh::InitAllMeshes(const aiScene *scene)
{
    for (u32 mesh_index = 0; mesh_index < m_Meshes.size(); ++mesh_index)
    {
        const aiMesh *mesh = scene->mMeshes[mesh_index];
        InitSingleMesh(mesh_index, mesh);
    }
}

void SkinnedMesh::InitSingleMesh(u32 mesh_index, const aiMesh *mesh)
{
    const aiVector3D zero_vec3(0.0f, 0.0f, 0.0f);

    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D &pos = mesh->mVertices[i];
        m_Positions.push_back({ pos.x, pos.y, pos.z });

        if (mesh->mNormals)
        {
            const aiVector3D &normal = mesh->mNormals[i];
            m_Normals.push_back({ normal.x, normal.y, normal.z });
        }
        else
        {
            aiVector3D normal(0.0f, 1.0f, 0.0f);
            m_Normals.push_back({ normal.x, normal.y, normal.z });
        }

        const aiVector3D &texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero_vec3;
        m_TexCoords.push_back({ texcoord.x, texcoord.y });
    }

    LoadMeshBones(mesh_index, mesh);

    // populate index buffer
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace &face = mesh->mFaces[i];
        m_Indices.push_back(face.mIndices[0]);
        m_Indices.push_back(face.mIndices[1]);
        m_Indices.push_back(face.mIndices[2]);
    }
}

void SkinnedMesh::LoadMeshBones(u32 mesh_index, const aiMesh *mesh)
{
    for (u32 i = 0; i < mesh->mNumBones; ++i)
    {
        LoadSingleBone(mesh_index, mesh->mBones[i]);
    }
}

void SkinnedMesh::LoadSingleBone(u32 mesh_index, const aiBone *bone)
{
    i32 bone_id = GetBoneId(bone);

    for (u32 i = 0; i < bone->mNumWeights; ++i)
    {
        const aiVertexWeight &vertex_weight = bone->mWeights[i];
        u32 global_vertex_id = m_Meshes[mesh_index].base_vertex + bone->mWeights[i].mVertexId;
        m_Bones[global_vertex_id].AddBoneData(bone_id, vertex_weight.mWeight);
    }
}

i32 SkinnedMesh::GetBoneId(const aiBone *bone)
{
    std::string bone_name(bone->mName.C_Str());
    if (m_BoneNameToIndexMap.find(bone_name) == m_BoneNameToIndexMap.end())
    {
        const i32 bone_index = static_cast<i32>(m_BoneNameToIndexMap.size());
        m_BoneNameToIndexMap[bone_name] = bone_index;
        return bone_index;
    }
    
    return m_BoneNameToIndexMap[bone_name];
}

bool SkinnedMesh::InitMaterials(const aiScene *scene, const std::string &filename)
{
    return true;
}

void SkinnedMesh::PopulateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const void *)0);

    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
        (const void *)(MAX_NUM_BONES_PER_VERTEX * sizeof(i32)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

void SkinnedMesh::LoadTextures(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadDiffuseTexture(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadSpecularTexture(const std::string &dir, const aiMaterial *material, i32 index)
{

}

void SkinnedMesh::LoadColors(const aiMaterial *material, i32 index)
{

}

