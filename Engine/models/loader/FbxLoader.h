#pragma once

// @see - https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_getting_started_installing_and_configuring_configuring_the_fbx_sdk_for_wind_html
#include <fbxsdk.h>

#include "AbstractLoader.h"
#include "../../modelclass.h"
#include "../actor/Actor.h"

class FbxLoader : public AbstractLoader
{
public:
    bool load(char*, ModelClass*);

protected:
    void loadAnimations(FbxScene* scene, FbxMesh* mesh, Actor* actor);
    
    unsigned int FindJointIndexUsingName(const std::string& inJointName);
    void FbxLoader::ProcessSkeletonHierarchy(FbxNode* inRootNode);
    void FbxLoader::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);  
    D3DXMATRIX toD3DXMATRIX(FbxAMatrix matrix);
    D3DXMATRIX toD3DXMATRIX(FbxMatrix matrix);
    FbxAMatrix convertToLeftHanded(FbxAMatrix fbxMatrix);
    FbxMatrix ConvertMatrix(FbxAMatrix Matrix);

protected:
    ModelClass* m_model;
    Actor::Animation m_animation;
    
};