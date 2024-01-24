#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "TextureClass.h"
#include <fstream>
#include <vector>

using namespace std;
using namespace DirectX;

class ObjectClass  // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
    enum PrimitiveType
    {
        Cube = 0,
        Sphere,
        Plane,
        Quad,
        Capsule,
        Cylinder,
        NonPrimitive,
    };
    //This is here so we can determine where to render (eg when the depth buffer is turned off and etc.)
    enum ObjectType
    {
        ThreeDimensional,
        TwoDimensional,
        UserInterface,
    };
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent; 
        XMFLOAT3 binormal;
    };
    struct ModelInformation
    {
        float x,y,z;
        float tu,tv;
        float nx,ny,nz;
        float tx,ty,tz;
        float bx,by,bz;
    };
    struct TempVertexType
    {
        float x, y, z;
        float tu, tv;
    };
public:
    ObjectClass();
    ObjectClass(const ObjectClass& a_copy);
    ~ObjectClass();

    bool Initialize(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, const char* a_modelFileName, char* a_textureFileNames[], int a_textureCount);
    bool InitializePrimitive(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, PrimitiveType a_primitive, char* a_textureFileNames[], int a_textureCount);
    bool Initialize2DQuad(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, int a_screenWidth, int a_screenHeight, int a_renderX, int a_renderY, char* a_textureFileNames[], int a_textureCount, int a_bitmapWidth = 0, int a_bitmapHeight = 0);
    void SetAsObjectToRender(ID3D11DeviceContext* a_deviceContext) const;

    int GetIndexCount() const;
    int GetVertexCount() const;
    int GetTextureCount() const;
    ModelInformation* GetModelData() const;
    
    ID3D11ShaderResourceView* GetTexture(int a_texture = 0) const;
    bool SetNewTextureAtId(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, int a_texId, char* a_textureFileName);

    void RemoveTextureFromFront();
    void RemoveTextureFromBack();

    XMFLOAT3 GetPosition() const;
    XMFLOAT3 GetRotation() const;
    XMFLOAT3 GetScale() const;

    void SetPosition(XMFLOAT3 a_value);
    void SetRotation(XMFLOAT3 a_value);
    void SetScale(XMFLOAT3 a_value);

    bool Update2DBuffers(ID3D11DeviceContext* a_context);

    ObjectType GetObjectType() const;
    
    void Shutdown();
    
private:
    
    bool LoadModelTxt(const char* a_modelFileName);
    bool LoadModelObj(const char* a_modelFileName);
    bool LoadModelFbx(const char* a_modelFileName);
    bool Initialize3DBuffers(ID3D11Device* a_device);
    void SetVertexIndexBuffers(ID3D11DeviceContext* a_deviceContext) const;

    bool AddTextureToModel(ID3D11Device* a_device, ID3D11DeviceContext* a_deviceContext, char* a_textureName);
    void CalculateModelVectors();
    static void CalculateTangentBinormal(const TempVertexType& a_vertex1, const TempVertexType& a_vertex2, const TempVertexType& a_vertex3, XMFLOAT3& a_tangent, XMFLOAT3& a_binormal);
    
    void ShutdownBuffers();
    void ReleaseTexture();
    void ReleaseModel();

public:
    
private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMFLOAT3 m_scale;
    
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    
    int m_vertexCount;
    int m_indexCount;

    int m_screenWidth, m_screenHeight;
    int m_2DWidth, m_2DHeight;
    int m_prevPosX, m_prevPosY;

    //Keeping as a list since right now texture amounts are unknown
    vector<TextureClass*> m_textures;

    ModelInformation* m_model;
    ObjectType m_objectType;
};