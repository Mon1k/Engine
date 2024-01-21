#pragma once

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../textures/Texture3DShaderClass.h"
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

    void set3D(bool is3D)
    {
        m_is3D = is3D;
    }

private:
    TextureShaderClass* m_TextureShader;
    Texture3DShaderClass* m_Texture3DShader;
    BitmapClass* m_Bitmap;

    bool m_is3D;
};