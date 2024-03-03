#ifndef _LABEL_H_
#define _LABEL_H_

#include "AbstractGUI.h"

#include "../textclass.h"

class Label: public AbstractGui
{
public:
    Label();
    Label(const Label&);
    ~Label();

    bool Initialize(int width, int height);
    virtual void Shutdown();
    virtual bool Render();

    bool Add(char*, int positionX, int positionY, float, float, float);
    bool Add(char*, int positionX, int positionY);
    bool Add(char*);
    

private:
    TextClass* m_Text;
};

#endif