// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
using namespace origin;

const i32 flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

int main()
{
    auto logger = Log();
    logger.PrintMessage(Info, "Hello World");

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("data/cube.gltf", flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OGN_CORE_ASSERT(false, importer.GetErrorString());
        return 1;
    }

    return 0;
}