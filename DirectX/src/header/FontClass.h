#pragma once

#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

#include "TextureClass.h"

class FontClass
{
private:
    struct FontType
    {
        //left right = uv coords, size = width in pixels.
        float left;
        float right;
        int size;
    };

    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };
    
public:

    FontClass();
    FontClass(const FontClass& a_Copy);
    ~FontClass();

    bool Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_Context, int a_FontChoice);
    void ShutDown();
    ID3D11ShaderResourceView* GetTexture();
    void BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);
    int GetSentencePixelLength(char*);
    int GetFontHeight();

private:
    bool LoadFontData(char* a_Filepath);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_Context, char* a_Filename);
    void ReleaseTexture();

private:
    FontType* m_Font;
    TextureClass* m_Texture;
    float m_fontHeight;
    int m_spaceSize;
};
