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
    virtual void update();

    std::string getText()
    {
        return m_Text->getText();
    }

    void setAlign(int align)
    {
        m_align = align;
    }

    bool Add(std::string, int positionX, int positionY, float, float, float);
    bool Add(std::string, int positionX, int positionY);
    bool Add(std::string);
    
    enum {
        ALIGN_LEFT = 0,
        ALIGN_RIGHT = 1,
        ALIGN_CENTER = 2
    };

private:
    TextClass* m_Text;
    int m_align;
};

#endif