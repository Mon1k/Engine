#pragma once

#include <fbxsdk.h>

#include "AbstractLoader.h"
#include "../../modelclass.h"

class FbxLoader : public AbstractLoader
{
public:
    bool load(char*, ModelClass*);
};