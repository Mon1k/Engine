#ifndef _ABSTRACTRESOURCE_H_
#define _ABSTRACTRESOURCE_H_

#include "../AbstractNode.h"

class AbstractResource: public AbstractNode
{
public:
    virtual void Shutdown() = 0;
    
};

#endif