﻿#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_InputLayout = 0;
    m_MatrixBuffer = 0;
    m_SampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& a_Copy)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle)
{
    bool result = true;
    wchar_t vsFileName[128];
    wchar_t psFileName[128] = L"";
    int error;
    
    error = wcscpy_s(vsFileName, 128, L"./src/shaders/textureVS.hlsl");
    if (error != 0)
    {
        return false;
    }
    error = wcscpy_s(vsFileName, 128, L"./src/shaders/texturePS.hlsl");
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
                          XMMATRIX a_Projection, ID3D11ShaderResourceView* a_ShaderResourceView)
{
    bool result;

    //set shader params that will be used for rendering
    result = SetShaderParams(a_DeviceContext, a_World, a_View, a_Projection, a_ShaderResourceView);

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
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;

    //init the pointers this func will use to null
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    result = D3DCompileFromFile(a_vsFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(a_psFileName, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;
    
    polygonLayout[1].SemanticName = "TEXCOORD0";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

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
                                   XMMATRIX a_Projection, ID3D11ShaderResourceView* a_Texture)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    MatrixBufferType* dataPtr;
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
