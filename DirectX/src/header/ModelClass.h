#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "TextureClass.h"
#include <fstream>

using namespace std;
using namespace DirectX;

class ModelClass
{
private:
    struct VertexType
    {
        //8
        XMFLOAT3 position;
        XMFLOAT3 color;
        XMFLOAT2 texture;

        //4
        XMFLOAT2 blendTexture1;
        XMFLOAT2 blendTexture2;

        //12
        XMFLOAT3 normal;
        XMFLOAT3 tangent; 
        XMFLOAT3 binormal;
        XMFLOAT3 padding;
        //24 bytes?
    };
    struct ModelType
    {
        float x,y,z;
        float tu,tv;
        float nx,ny,nz;
    };
    
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName, char* a_ModelFileName);
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();

    ID3D11ShaderResourceView* GetTexture();
    
private:
    bool LoadModel(char* a_ModelFileName);
    void ReleaseModel();
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
    ModelType* m_Model;
};
#endif