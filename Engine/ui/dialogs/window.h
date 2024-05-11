#pragma once

#include "../AbstractGUI.h"
#include "../group.h"
#include "../label.h"

#include "../../textureshaderclass.h"
#include "../../bitmapclass.h"


#include <string>

class Window : public Group
{
public:
    enum EventType : int {
        WINDOW_CLOSE = 65537,
        WINDOW_OPEN = 65538
    };

    static const int HEADER_BUTTON_CLOSE = 1;
    static const int HEADER_BUTTON_MAXIMIZE = 2;
    static const int HEADER_BUTTON_MINIMIZE = 4;

public:
    Window();
    ~Window();

    bool Initialize(int width, int height, int positionX, int positionY);
    bool addHeader(std::string title, int flags);
    bool addBody();

    virtual void Shutdown();
    virtual bool Render();
    virtual AbstractGui* addChild(AbstractGui*);
    void setTitle(std::string);
    virtual void show();
    virtual void close();
    
    Group* getHeader() {
        return m_header;
    }
    
    Group* getBody() {
        return m_body;
    }

protected:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_BitmapHeader;
    BitmapClass* m_BitmapBody;

    Label* m_title;
    Group* m_header;
    Group* m_body;
};