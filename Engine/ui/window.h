#pragma once

#include "AbstractGUI.h"
#include "group.h"
#include "label.h"
#include "button.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"


#include <string>
using namespace std;

class Window : public Group
{
public:
    Window();
    ~Window();

    bool Initialize(int, int, WCHAR*, char*, int, int, int, int);
    void Shutdown();
    bool Render();
    void addChild(AbstractGui*);

protected:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;

    Group* m_header;
    Group* m_body;
};