#include "..\header\Shader.h"
#include <d3dcompiler.h>
#include <fstream>

#include "ApplicationClass.h"

Shader::Shader(const WCHAR* a_vertexShaderFilePath, const WCHAR* a_pixelShaderFilePath, const char* a_vertexShaderEntryPoint, const char* a_pixelShaderEntryPoint)
{
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    ID3D10Blob* errorMessage;
    
    m_result = D3DCompileFromFile(a_vertexShaderFilePath, nullptr, nullptr, a_vertexShaderEntryPoint, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (!CheckShaderCompileResult(errorMessage, a_vertexShaderFilePath))
    {
        errorMessage->Release();
        delete errorMessage;
        return;
    }
    
    m_result = D3DCompileFromFile(a_pixelShaderFilePath, nullptr, nullptr, a_pixelShaderEntryPoint, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (!CheckShaderCompileResult(errorMessage, a_vertexShaderFilePath))
    {
        errorMessage->Release();
        delete errorMessage;
        return;
    }

    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(m_result))
        return;
    
    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
    if (FAILED(m_result))
        return;

    CreateDefaultInputLayoutDescription();
    //I have no idea why rider is giving me an error for m_inputElementDescriptions and count when the function before this defines and inits them.
    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreateInputLayout(m_inputElementDescriptions, m_inputLayoutCount, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_inputLayout);
    if (FAILED(m_result))
        return;

    CreateDefaultSamplerDescription();
    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreateSamplerState(&m_samplerDescription, &m_samplerState);
    if (FAILED(m_result))
        return;
    
    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
    if (errorMessage)
        errorMessage->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer = nullptr;
    errorMessage = nullptr;
}

Shader::~Shader()
{
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }
    while (m_vsShaderBuffers.size() > 0)
    {
        m_vsShaderBuffers.back()->Release();
        m_vsShaderBuffers.pop_back();
    }
    while (m_psShaderBuffers.size() > 0)
    {
        m_psShaderBuffers.back()->Release();
        m_psShaderBuffers.pop_back();
    }
    if (m_inputLayout)
    {
        m_inputLayout->Release();
        m_inputLayout = nullptr;
    }
    
    delete[] m_inputElementDescriptions;
    
    if (m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = nullptr;
    }
}

void Shader::RenderShader(int a_indexCount) const
{
    //set the vertex input layout
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->IASetInputLayout(m_inputLayout);
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->PSSetSamplers(0, 1, &m_samplerState);
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->DrawIndexed(a_indexCount, 0, 0);
}

void Shader::SetShaderResources(UINT a_index, ID3D11ShaderResourceView* a_resource)
{
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->PSSetShaderResources(a_index, 1, &a_resource);
}

bool Shader::SetBufferData(UINT a_index, void* a_data, UINT a_byteAmount, bool a_vertexShader) 
{
    m_result = ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->Map(
        a_vertexShader ? m_vsShaderBuffers[a_index] : m_psShaderBuffers[a_index],
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &m_mappedSubresource);

    if (FAILED(m_result))
        return false;
    memcpy(m_mappedSubresource.pData, a_data, a_byteAmount);
    
    ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->Unmap(
        a_vertexShader ? m_vsShaderBuffers[a_index] : m_psShaderBuffers[a_index],
        0);
    if (a_vertexShader)
        ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->VSSetConstantBuffers(a_index, 1, &m_vsShaderBuffers[a_index]);
    else
        ApplicationClass::Instance->m_Direct3D->GetDeviceContext()->PSSetConstantBuffers(a_index, 1, &m_psShaderBuffers[a_index]);
    
    return true;
}

bool Shader::CreateDynamicBuffer(UINT a_byteSize, bool a_vertexBuffer)
{
    ID3D11Buffer* newBuffer;
    if (a_vertexBuffer)
        m_vsShaderBuffers.push_back(newBuffer);
    else
        m_psShaderBuffers.push_back(newBuffer);
    
    ResetBufferDescription();
    m_bufferDesc.ByteWidth = a_byteSize;
    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreateBuffer(&m_bufferDesc, nullptr, a_vertexBuffer ? &m_vsShaderBuffers.back() : &m_psShaderBuffers.back());
    if (FAILED(m_result))
        return false;

    return true;
}

//Since the initialization
bool Shader::FailedAction() const
{
    return FAILED(m_result);
}

bool Shader::CreateDefaultSamplerDescription()
{
    //Filter is the most important element of the sampler. it tells the sampler how to pick what pixels to be used or combine to create the final look of the texture on the polygon face.
    //the filter below is more expensive but gives a good(it says best but i am not sure about that) visual result. it tells the sampler to use lin-interp for minification, magnification and mip-level sampling.
    //the wrapping for uv set so its never greater than 0-1 anything outside of that wraps around and the rest of these settings are defaults for the sampler.
    m_samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDescription.MipLODBias = 0.0f;
    m_samplerDescription.MaxAnisotropy = 1;
    m_samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    m_samplerDescription.BorderColor[0] = 0;
    m_samplerDescription.BorderColor[1] = 0;
    m_samplerDescription.BorderColor[2] = 0;
    m_samplerDescription.BorderColor[3] = 0;
    m_samplerDescription.MinLOD = 0;
    m_samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
    m_result = ApplicationClass::Instance->m_Direct3D->GetDevice()->CreateSamplerState(&m_samplerDescription, &m_samplerState);
    return !(FAILED(m_result));
}

void Shader::SetInputDescriptionElement(UINT a_index, const char* a_semanticName, DXGI_FORMAT a_format) const
{
    m_inputElementDescriptions[a_index].SemanticName = a_semanticName;
	m_inputElementDescriptions[a_index].SemanticIndex = 0;
	m_inputElementDescriptions[a_index].Format = a_format;
	m_inputElementDescriptions[a_index].InputSlot = 0;
    
    if (a_index == 0)
	    m_inputElementDescriptions[a_index].AlignedByteOffset = 0;
    else
	    m_inputElementDescriptions[a_index].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    
	m_inputElementDescriptions[a_index].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	m_inputElementDescriptions[a_index].InstanceDataStepRate = 0;
}

void Shader::CreateDefaultInputLayoutDescription()
{
    m_inputElementDescriptions = new D3D11_INPUT_ELEMENT_DESC[5];
    m_inputLayoutCount = 5;
    SetInputDescriptionElement(0, "POSITION");
    //TEXCOORD is the only input in the default that isn't a RGB32 input but a RG32
    SetInputDescriptionElement(1, "TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
    SetInputDescriptionElement(2, "NORMAL");
    SetInputDescriptionElement(3, "TANGENT");
    SetInputDescriptionElement(4, "BINORMAL");
}

void Shader::ResetBufferDescription()
{
    m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_bufferDesc.ByteWidth = 0; 
    m_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_bufferDesc.MiscFlags = 0;
    m_bufferDesc.StructureByteStride = 0;
}

bool Shader::CheckShaderCompileResult(ID3D10Blob* a_errorMessage, const WCHAR* a_shaderFileName) const
{
    if (FAILED(m_result))
    {
        //if the shader failed to compile there should be a message
        if (a_errorMessage)
        {
            ofstream inOutStream;
            const char* compileErrors = (char*)(a_errorMessage->GetBufferPointer());
            const unsigned long long bufferSize = a_errorMessage->GetBufferSize();

            inOutStream.open("ShaderError.txt");
            for (int i = 0; i < bufferSize; i++)
                inOutStream << compileErrors[i];
            
            inOutStream.close();
            MessageBox(ApplicationClass::Instance->m_windowHandle, L"Error compiling shader. Check ShaderError.txt for message.", a_shaderFileName, MB_OK);
        }
        //if no error message usually means file could not be found
        else
            MessageBox(ApplicationClass::Instance->m_windowHandle, a_shaderFileName, L"Missing Shader File", MB_OK);
        return false;
    }
    return true;
}