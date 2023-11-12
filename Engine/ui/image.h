#pragma once

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"

class Image : public AbstractGui
{
public:
    Image();
    ~Image();

    bool Initialize(int, int, int, int);
    void loadTextureByString(std::string path);
    void loadTextureByResource(ID3D11ShaderResourceView* texture);
    void Shutdown();
    bool Render();

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
};