#pragma once

// @see - https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_getting_started_installing_and_configuring_configuring_the_fbx_sdk_for_wind_html
//#include <fbxsdk.h>

#include "assimp/matrix4x4.h"
#include "assimp/scene.h"

#include <map>

#include "AbstractLoader.h"
#include "../../modelclass.h"
#include "../actor/Actor.h"

class FbxLoader : public AbstractLoader
{
public:
    bool load(char*, ModelClass*);

protected:
    unsigned int FindJointIndexUsingName(const std::string& inJointName);
    D3DXMATRIX toD3DXMATRIX(aiMatrix4x4 matrix);

    int GetBoneId(std::string boneName);
    void createTreeNode(aiNode* node, Actor* actor, Actor::NodeInfo* parent, int depth = 0);
    Actor::NodeInfo* FindNode(Actor* actor, Actor::BoneInfo bone);

protected:
    ModelClass* m_model;
    Actor::Animation m_animation;
    const aiScene* m_Scene;

    std::map<string, int> m_BoneNameToIndexMap;
    
};