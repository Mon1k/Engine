#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

#include "AbstractLoader.h"
#include "../AbstractModel.h"

class ObjLoader : public AbstractLoader
{
public:
    bool load(char*, AbstractModel*);
};

#endif