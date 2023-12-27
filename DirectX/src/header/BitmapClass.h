#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "TextureClass.h"

class BitmapClass
{
public:

    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };
    
    BitmapClass();
    BitmapClass(const BitmapClass&);
    ~BitmapClass();

    bool Initialize(ID3D11Device* a_Device,
                    ID3D11DeviceContext* a_DeviceContext,
                    int a_ScreenWidth,
                    int a_ScreenHeight,
                    char* a_TextureFilePath,
                    int a_RenderX,
                    int a_RenderY);
    void Shutdown();
    bool Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();

    void UpdateWidth(int a_Width) {m_bitmapWidth = a_Width;}
    void UpdateHeight(int a_Height) {m_bitmapHeight = a_Height;}

    void SetRenderLocation(int a_PosX, int a_PosY);

private:
    bool InitializeBuffers(ID3D11Device* a_Device);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext* a_DeviceContext);
    void RenderBuffers(ID3D11DeviceContext* a_DeviceContext);

    bool LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FilePath);
    void ReleaseTexture();

//The BitmapClass will need to maintain some extra information that a 3D model wouldn't such as
//the screen size,
//the bitmap size,
//and the last place it was rendered. We have added extra private variables here to track that extra information.
private:
    ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
    int m_vertexCount, m_IndexCount;
    int m_screenWidth, m_screenHeight;
    int m_bitmapWidth, m_bitmapHeight;
    int m_RenderX, m_RenderY;
    int m_prevPosX, m_prevPosY;
    TextureClass* m_Texture;

};
