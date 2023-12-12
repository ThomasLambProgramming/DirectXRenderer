#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class ModelClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };
    
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device* a_Device);
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();
    
private:
    bool InitializeBuffers(ID3D11Device* a_Device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* a_DeviceContext);

private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    int m_VertexCount;
    int m_IndexCount;
};
#endif