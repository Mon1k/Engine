#pragma once

#include "assimp/scene.h"

#include "AbstractLoader.h"
#include "../../modelclass.h"
#include "../actor/Actor.h"

class AssimpLoader : public AbstractLoader
{
public:
    bool load(char*, ModelClass*);

protected:
    void createTreeNode(aiNode* node, Actor* actor, Actor::NodeInfo* parent, int depth = 0);
    Actor::NodeInfo* FindNode(Actor* actor, Actor::BoneInfo bone);

protected:
    const aiScene* m_Scene;
};