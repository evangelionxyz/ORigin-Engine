// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include <Origin.h>

#include "Origin/Audio/FmodSound.h"
using namespace origin;

const i32 flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals 
    | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

void PrintNodeHierarchy(const aiNode* node, int indentLevel, Log& logger)
{
    std::string indent(indentLevel * 2, ' ');

    logger.PrintMessage(indent + node->mName.C_Str());

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        PrintNodeHierarchy(node->mChildren[i], indentLevel + 1, logger);
    }
}

void test_one(Log &logger)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("data/untitled.glb", flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OGN_CORE_ASSERT(false, importer.GetErrorString());
        return;
    }

    // Print the hierarchy of the model starting from the root node
    logger.PrintMessage("Model Hierarchy:");
    PrintNodeHierarchy(scene->mRootNode, 0, logger);
}

void test_two(Log &logger)
{
    Ref<StaticModel> model = CreateRef<StaticModel>("data/barbarian.glb");
    for (auto& m : model->GetMeshes())
    {
        logger.PrintMessage(m->Name);
    }
}

void test_audio_engine()
{
    FmodAudio audio_engine;

    const Ref<FmodSound> sound_a = audio_engine.CreateSound("bullet", "data/loading_screen.wav", FMOD_DEFAULT);
    sound_a->Play();

    while (true)
    {
        audio_engine.Update();
    }
}

int main()
{
    auto logger = Log();
    logger.PrintMessage("Hello World");

    test_audio_engine();

    return 0;
}