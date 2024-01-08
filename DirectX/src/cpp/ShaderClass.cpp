#include "ShaderClass.h"

ShaderClass::ShaderClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_InputLayout = 0;
    m_SampleState = 0;
    m_MatrixBuffer = 0;
    m_LightInformationBuffer = 0;
    m_PixelBuffer = 0;
    m_Texture = 0;
    m_BlendTexture1 = 0;
    m_BlendTexture2 = 0;
    m_AllowLights = true;
}

ShaderClass::ShaderClass(const ShaderClass& a_Copy)
{
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, int a_blendAmount, bool a_allowLights)
{
    bool result;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    int error;

    m_AllowLights = a_allowLights;
    
    error = wcscpy_s(vsFileName, 128, L"./src/shaders/textureVS.hlsl");
    if (error != 0)
    {
        return false;
    }
    error = wcscpy_s(psFileName, 128, L"./src/shaders/texturePS.hlsl");
    if (error != 0)
    {
        return false;
    }
    result = InitializeShader(a_Device, a_WindowHandle, vsFileName, psFileName, a_blendAmount);

    if (!result)
    {
        return false;
    }
    return true;
}

void ShaderClass::Shutdown()
{
    //shutdown vert and pixel shaders + any other related objects shutdown.
    ShutdownShader();
}

bool ShaderClass::Render(ID3D11DeviceContext* a_deviceContext,
                int a_indexCount,
                ID3D11ShaderResourceView* a_texture1,
                ID3D11ShaderResourceView* a_texture2,
                ID3D11ShaderResourceView* a_texture3,
                XMMATRIX a_world,
                XMMATRIX a_view,
                XMMATRIX a_projection,
                XMFLOAT4 a_lightDiffuse,
                XMFLOAT3 a_lightDirection)
{
    bool result;

    //set shader params that will be used for rendering
    result = SetShaderParams(a_deviceContext, a_texture1, a_texture2, a_texture3, a_world, a_view, a_projection, a_lightDiffuse, a_lightDirection);
    if (!result)
        return false;

    //Now render prepared buffers with the shader
    RenderShader(a_deviceContext, a_indexCount);
    return true;
}

ID3D11ShaderResourceView* ShaderClass::GetTexture(int a_textureNumber)
{
    switch(a_textureNumber)
    {
        case 0:
            return m_Texture;
        case 1:
            return m_BlendTexture1;
        case 2:
            return m_BlendTexture2;
        default:
            return m_Texture;
    }
}

bool ShaderClass::AllowsLights()
{
    return m_AllowLights;
}

bool ShaderClass::HasBlendingEnabled()
{
    return m_BlendTexture1 || m_BlendTexture2;
}

bool ShaderClass::InitializeShader(ID3D11Device* a_device, HWND a_windowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName, int a_amountOfBlendTextures)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightInfoBufferDesc;
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
            OutputShaderErrorMessage(errorMessage, a_windowHandle, a_vsFileName);
        //if there is no error message then it probably means that the file could not be found
        else
            MessageBox(a_windowHandle, a_vsFileName, L"Missing Shader File", MB_OK);
        return false;
    }
    
    if (a_amountOfBlendTextures == 1)
        result = D3DCompileFromFile(a_psFileName, NULL, NULL, "TextureMultiSamplePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    else if (a_amountOfBlendTextures == 2)
        result = D3DCompileFromFile(a_psFileName, NULL, NULL, "TextureAlphaMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    else if (a_amountOfBlendTextures == 3)
        result = D3DCompileFromFile(a_psFileName, NULL, NULL, "NormalMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    //Im an idiot and will probably accidentally add 3 blend textures and it will fail so default to no blending in that case.
    else
        result = D3DCompileFromFile(a_psFileName, NULL, NULL, "TextureSingleSamplePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
        
        
    if (FAILED(result))
    {
        //if the shader failed to compile there should be a message
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, a_windowHandle, a_psFileName);
        //if there is no error message then it probably means that the file could not be found
        else
            MessageBox(a_windowHandle, a_psFileName, L"Missing Shader File", MB_OK);
        return false;
    }

    result = a_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    if (FAILED(result))
    {
        return false;
    }
    
    result = a_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
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

    polygonLayout[3].SemanticName = "TANGENT";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;

    polygonLayout[4].SemanticName = "BINORMAL";
    polygonLayout[4].SemanticIndex = 0;
    polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[4].InputSlot = 0;
    polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[4].InstanceDataStepRate = 0;

    
    //size of array / individual element size
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = a_device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout);
    if (FAILED(result))
    {
        return false;
    }

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    
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
    result = a_device->CreateSamplerState(&samplerDesc, &m_SampleState);
    if (FAILED(result))
    {
        return false;
    }

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    //create the constant buffer pointer so we can access the vertex shader constant buffer from within the class
    result = a_device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    lightInfoBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightInfoBufferDesc.ByteWidth = sizeof(LightInformationBufferType);
    lightInfoBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightInfoBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightInfoBufferDesc.MiscFlags = 0;
    lightInfoBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&lightInfoBufferDesc, NULL, &m_LightInformationBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}




void ShaderClass::ShutdownShader()
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
    if (m_PixelBuffer)
    {
        m_PixelBuffer->Release();
        m_PixelBuffer = 0;
    }
    if (m_LightInformationBuffer)
    {
        m_LightInformationBuffer->Release();
        m_LightInformationBuffer = 0;
    }
    if (m_BlendTexture1)
    {
        m_BlendTexture1->Release();
        m_BlendTexture1 = 0;
    }
    if (m_BlendTexture2)
    {
        m_BlendTexture2->Release();
        m_BlendTexture2 = 0;
    }
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = 0;
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
}

bool ShaderClass::SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                        ID3D11ShaderResourceView* a_Texture1,
                        ID3D11ShaderResourceView* a_Texture2,
                        ID3D11ShaderResourceView* a_Texture3,
                        XMMATRIX a_world,
                        XMMATRIX a_view,
                        XMMATRIX a_projection,
                        XMFLOAT4 a_lightDiffuse,
                        XMFLOAT3 a_lightDirection)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    MatrixBufferType* matrixDataPtr;
    LightInformationBufferType* lightInformationDataPtr;
    
    //I believe this has something to do with the register(t0) and etc. look this up later.
    a_DeviceContext->PSSetShaderResources(0,1, &a_Texture1);
    a_DeviceContext->PSSetShaderResources(1,1, &a_Texture2);
    a_DeviceContext->PSSetShaderResources(2,1, &a_Texture3);
    
    //transpose the matrices to prepare them for the shader.
    a_world = XMMatrixTranspose(a_world);
    a_view = XMMatrixTranspose(a_view);
    a_projection = XMMatrixTranspose(a_projection);


    //Buffer 1 / MATRIX BUFFER-----------------------------------------
    //lock the constant buffer so we can write to it.
    result = a_DeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }
    //get a pointer to the data in the constant buffer
    matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;
    //copy the data into the buffer
    matrixDataPtr->world = a_world;
    matrixDataPtr->view = a_view;
    matrixDataPtr->projection = a_projection;
    a_DeviceContext->Unmap(m_MatrixBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(0, 1, &m_MatrixBuffer);
    //End Buffer 1----------------------------------------------------

    
    //Buffer 2 / LIGHT INFO BUFFER-----------------------------------------
    result = a_DeviceContext->Map(m_LightInformationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }
    lightInformationDataPtr = (LightInformationBufferType*)mappedSubresource.pData;

    lightInformationDataPtr->diffuseColor = a_lightDiffuse;
    lightInformationDataPtr->lightDirection = a_lightDirection;
    lightInformationDataPtr->padding = 0.0f;
    
    a_DeviceContext->Unmap(m_LightInformationBuffer, 0);
    //As the pixel shader has the exact same setup we can just give the information here.
    a_DeviceContext->PSSetConstantBuffers(0, 1, &m_LightInformationBuffer);
    //End Buffer 2----------------------------------------------------

    return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount)
{
    //set the vertex input layout
    a_DeviceContext->IASetInputLayout(m_InputLayout);
    a_DeviceContext->VSSetShader(m_VertexShader, NULL, 0);
    a_DeviceContext->PSSetShader(m_PixelShader, NULL, 0);
    a_DeviceContext->PSSetSamplers(0, 1, &m_SampleState);
    a_DeviceContext->DrawIndexed(a_IndexCount, 0, 0);
}
void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* a_FilePath)
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
}
