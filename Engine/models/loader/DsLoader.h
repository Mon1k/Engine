#ifndef _DSLOADER_H_
#define _DSLOADER_H_

#include "AbstractLoader.h"
#include "../AbstractModel.h"

class DsLoader: public AbstractLoader
{
public:
    bool load(char* , AbstractModel*);
};

#endif