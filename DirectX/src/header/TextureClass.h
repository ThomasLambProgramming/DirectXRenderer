#pragma once

#include <d3d11.h>
#include <stdio.h>

class TextureClass
{
    
private:
    struct TargaHeader
    {
        //unsigned char, which gives you at least the 0 to 255 range. This might be useful for displaying an octet e.g. as hex value. (yeah wack.)
        unsigned char data1[12];
        //unsigned char = 0-255, unsigned short = 0-65535
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };
    
public:
    TextureClass();
    TextureClass(const TextureClass& a_Copy);
    ~TextureClass();

    bool Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    int GetWidth();
    int GetHeight();

private:
    bool LoadTarga32Bit(char* a_FileName);

private:
    //raw targa data
    unsigned char* m_TargaData;
    //structured data for directx
    ID3D11Texture2D* m_Texture;
    //Is the resource view that the shader uses to access the texture data when drawing
    ID3D11ShaderResourceView* m_TextureView;
    //texture dimensions.
    int m_Width;
    int m_Height;
};
