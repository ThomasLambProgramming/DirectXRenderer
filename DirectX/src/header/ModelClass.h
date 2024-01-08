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
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent; 
        XMFLOAT3 binormal;
    };
    struct ModelType
    {
        float x,y,z;
        float tu,tv;
        float nx,ny,nz;
        float tx, ty, tz;
        float bx, by, bz;
    };
    struct TempVertexType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct VectorType
    {
        float x, y, z;
    };
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_TextureFileName, char* a_ModelFileName, char* a_BlendTextureFileName1);
    void Shutdown();
    void Render(ID3D11DeviceContext* a_DeviceContext);

    int GetIndexCount();

    ID3D11ShaderResourceView* GetTexture(int a_texture = 0);
    
private:
    bool LoadModel(char* a_ModelFileName);
    void ReleaseModel();
    bool InitializeBuffers(ID3D11Device* a_Device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* a_DeviceContext);

    bool LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName, int a_texId);
    void ReleaseTexture();
    
    void CalculateModelVectors();
    void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    int m_VertexCount;
    int m_IndexCount;

    TextureClass* m_Texture;
    TextureClass* m_blendTexture1;
    ModelType* m_Model;
};
#endif