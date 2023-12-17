#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "TextureClass.h"

using namespace DirectX;

class ModelClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };
    
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName);
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();

    ID3D11ShaderResourceView* GetTexture();
    
private:
    bool InitializeBuffers(ID3D11Device* a_Device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* a_DeviceContext);

    bool LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName);
    void ReleaseTexture();
    
private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    int m_VertexCount;
    int m_IndexCount;

    TextureClass* m_Texture;
};
#endif