#pragma once
#include <dxgiformat.h>
#include "ApplicationClass.h"

class ShaderBase
{
public:
    ShaderBase(const WCHAR* a_vertexShaderFilePath, const WCHAR* a_pixelShaderFilePath, const char* a_vertexShaderEntryPoint, const char* a_pixelShaderEntryPoint);
    ~ShaderBase();
    //we dont ever want to be able to copy a shader, always make new and init or use the current shader.
    ShaderBase(const ShaderBase& a_copy) = delete;
    
    void RenderShader(int a_indexCount) const;
    
    //Set the textures for the shader 
    static void SetShaderResources(UINT a_index, ID3D11ShaderResourceView* a_resource);
    
    bool SetBufferData(UINT a_index, void* a_data, bool a_vertexShader);
    bool CreateDynamicBuffer(UINT a_byteSize, bool a_vertexBuffer);

    bool FailedAction() const;
    
private:
    bool CreateDefaultSamplerDescription();
    //Trying to reduce code lines, just looking to experiment with making compact shader classes
    void SetInputDescriptionElement(UINT a_index, const char* a_semanticName, DXGI_FORMAT a_format = DXGI_FORMAT_R32G32B32_FLOAT) const;
    void CreateDefaultInputLayoutDescription();
    void ResetBufferDescription();
    
    
    //Will output error message
    bool CheckShaderCompileResult(ID3D10Blob* a_errorMessage, const WCHAR* a_shaderFileName) const;

    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    
    std::vector<ID3D11Buffer*> m_vsShaderBuffers;
    std::vector<ID3D11Buffer*> m_psShaderBuffers;

    ID3D11InputLayout* m_inputLayout;
    D3D11_INPUT_ELEMENT_DESC* m_inputElementDescriptions;
    
    ID3D11SamplerState* m_samplerState;
    D3D11_SAMPLER_DESC m_samplerDescription;
    
    //creating one buffer desc to reuse/reset + not creating memory every frame.
    D3D11_BUFFER_DESC m_bufferDesc;
    D3D11_MAPPED_SUBRESOURCE m_mappedSubresource;
    UINT m_inputLayoutCount;
    
    HRESULT m_result;
};
