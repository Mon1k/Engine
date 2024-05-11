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
    void setValign(int valign)
    {
        m_valign = valign;
    }

    bool Add(std::string, int positionX, int positionY, float, float, float);
    bool Add(std::string, int positionX, int positionY);
    bool Add(std::string);
    
    enum {
        ALIGN_LEFT = 0,
        ALIGN_RIGHT = 1,
        ALIGN_CENTER = 2,
        
        VALIGN_TOP = 10,
        VALIGN_MIDDLE = 11,
        VALIGN_BOTTOM = 12,
    };

private:
    TextClass* m_Text;
    int m_align;
    int m_valign;
};

#endif