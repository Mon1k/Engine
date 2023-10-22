#pragma once

#include "assimp/matrix4x4.h"
#include "assimp/scene.h"

#include "AbstractLoader.h"
#include "../../modelclass.h"
#include "../actor/Actor.h"

class FbxLoader : public AbstractLoader
{
public:
    bool load(char*, ModelClass*);

protected:
    void createTreeNode(aiNode* node, Actor* actor, Actor::NodeInfo* parent, int depth = 0);
    Actor::NodeInfo* FindNode(Actor* actor, Actor::BoneInfo bone);

protected:
    Actor::Animation m_animation;
    const aiScene* m_Scene;
};