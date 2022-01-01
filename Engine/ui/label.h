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

    bool Initialize(int, int, int, int);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Add(char*, int, int);
    bool Render();

private:
    TextClass* m_Text;
};

#endif