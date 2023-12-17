﻿#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_InputLayout = 0;
    m_MatrixBuffer = 0;
    m_SampleState = 0;
    m_LightBuffer = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& a_Copy)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle)
{
    bool result;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    int error;
    
    error = wcscpy_s(vsFileName, 128, L"./src/shaders/lightvs.hlsl");
    if (error != 0)
    {
        return false;
    }
    error = wcscpy_s(psFileName, 128, L"./src/shaders/lightps.hlsl");
    if (error != 0)
    {
        return false;
    }
    result = InitializeShader(a_Device, a_WindowHandle, vsFileName, psFileName);

    if (!result)
    {
        return false;
    }
    return true;
}

void TextureShaderClass::Shutdown()
{
    //shutdown vert and pixel shaders + any other related objects shutdown.
    ShutdownShader();
    return;
}
bool TextureShaderClass::Render(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount, XMMATRIX a_World, XMMATRIX a_View,
                          XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView,  XMFLOAT3 a_LightDirection, XMFLOAT4 a_DiffuseColor)
{
    bool result;

    //set shader params that will be used for rendering
    result = SetShaderParams(a_DeviceContext, a_World, a_View, a_Projection, a_ShaderResourceView, a_LightDirection, a_DiffuseColor);

    if (!result)
    {
        return false;
    }

    //Now render prepared buffers with the shader
    RenderShader(a_DeviceContext, a_IndexCount);
    return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* a_vsFileName,
                                    WCHAR* a_psFileName)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;

    //init the pointers this func will use to null
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    result = D3DCompileFromFile(a_vsFileName, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        //if the shader failed to compile there should be a message
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, a_WindowHandle, a_vsFileName);
        }
        else
        {
            //if there is no error message then it probably means that the file could not be found
            MessageBox(a_WindowHandle, a_vsFileName, L"Missing Shader File", MB_OK);
        }
        return false;
    }
    result = D3DCompileFromFile(a_psFileName, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        //if the shader failed to compile there should be a message
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, a_WindowHandle, a_psFileName);
        }
        else
        {
            //if there is no error message then it probably means that the file could not be found
            MessageBox(a_WindowHandle, a_psFileName, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    result = a_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    if (FAILED(result))
    {
        return false;
    }
    
    result = a_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    if (FAILED(result))
    {
        return false;
    }

    polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    
    //size of array / individual element size
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = a_Device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout);
    if (FAILED(result))
    {
        return false;
    }

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    //create the constant buffer pointer so we can access the vertex shader constant buffer from within the class
    result = a_Device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    if (FAILED(result))
    {
        return false;
    }

    //Filter is the most important element of the sampler. it tells the sampler how to pick what pixels to be used or combine to create the final look of the texture on the polygon face.
    //the filter below is more expensive but gives a good(it says best but hmmm) visual result. it tells the sampler to use lin-interp for minification, magnifiction and mip-level sampling.
    //the wrapping for uv set so its never greater than 0-1 anything outside of that wraps around and the rest of these settings are defaults for the sampler.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = a_Device->CreateSamplerState(&samplerDesc, &m_SampleState);
    if (FAILED(result))
    {
        return false;
    }

    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    result = a_Device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);

    if (FAILED(result))
    {
        return false;
    }
    return true;
}




void TextureShaderClass::ShutdownShader()
{
    //release all pointers
    if (m_SampleState)
    {
        m_SampleState->Release();
        m_SampleState = 0;
    }
    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = 0;
    }
    if (m_InputLayout)
    {
        m_InputLayout->Release();
        m_InputLayout = 0;
    }
    if (m_VertexShader)
    {
        m_VertexShader->Release();
        m_VertexShader = 0;
    }
    if (m_PixelShader)
    {
        m_PixelShader->Release();
        m_PixelShader = 0;
    }
    if (m_LightBuffer)
    {
        m_LightBuffer->Release();
        m_LightBuffer = 0;
    }
    return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;

    //get pointer to the errer message text buffer
    compileErrors = (char*)(a_ErrorMessage->GetBufferPointer());

    //get length of the message
    bufferSize = a_ErrorMessage->GetBufferSize();

    //open a file to write the error message to.
    fout.open("ShaderError.txt");
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }
    fout.close();

    a_ErrorMessage->Release();
    a_ErrorMessage = 0;

    MessageBox(a_WindowHandle, L"Error compiling shader. Check ShaderError.txt for message.", a_FilePath, MB_OK);
    return;
}

bool TextureShaderClass::SetShaderParams(ID3D11DeviceContext* a_DeviceContext, XMMATRIX a_World, XMMATRIX a_View,
                                   XMMATRIX a_Projection, ID3D11ShaderResourceView* a_Texture,  XMFLOAT3 a_LightDirection, XMFLOAT4 a_DiffuseColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    MatrixBufferType* dataPtr;
    LightBufferType* dataPtr2;
    unsigned int bufferNumer;

    //transpose the matrices to prepare them for the shader.
    a_World = XMMatrixTranspose(a_World);
    a_View = XMMatrixTranspose(a_View);
    a_Projection = XMMatrixTranspose(a_Projection);

    //lock the constant buffer so we can write to it.
    result = a_DeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }

    //get a pointer to the data in the constant buffer
    dataPtr = (MatrixBufferType*)mappedSubresource.pData;

    //copy the data into the buffer
    dataPtr->world = a_World;
    dataPtr->view = a_View;
    dataPtr->projection = a_Projection;

    a_DeviceContext->Unmap(m_MatrixBuffer, 0);

    bufferNumer = 0;
    a_DeviceContext->VSSetConstantBuffers(bufferNumer, 1, &m_MatrixBuffer);
    a_DeviceContext->PSSetShaderResources(0,1, &a_Texture);


    result = a_DeviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }
    //get pointer to the data in the lighting constant buffer
    dataPtr2 = (LightBufferType*)mappedSubresource.pData;
    dataPtr2->lightDirection = a_LightDirection;
    dataPtr2->padding = 0.0f;
    dataPtr2->diffuseColor = a_DiffuseColor;
    a_DeviceContext->Unmap(m_LightBuffer, 0);
    bufferNumer = 0;
    a_DeviceContext->PSSetConstantBuffers(bufferNumer, 1, &m_LightBuffer);
    
    return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount)
{
    //set the vertex input layout
    a_DeviceContext->IASetInputLayout(m_InputLayout);
    a_DeviceContext->VSSetShader(m_VertexShader, NULL, 0);
    a_DeviceContext->PSSetShader(m_PixelShader, NULL, 0);
    a_DeviceContext->PSSetSamplers(0, 1, &m_SampleState);
    a_DeviceContext->DrawIndexed(a_IndexCount, 0, 0);
}
