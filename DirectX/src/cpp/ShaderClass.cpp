#include "ShaderClass.h"

ShaderClass::ShaderClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_InputLayout = 0;
    m_SampleState = 0;
    m_MatrixBuffer = 0;
    m_LightInformationBuffer = 0;
    m_CameraBuffer = 0;
    m_Texture = 0;
    m_SecondaryTexture1 = 0;
    m_SecondaryTexture2 = 0;
    m_AllowLights = true;
}

ShaderClass::ShaderClass(const ShaderClass& a_Copy)
{
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, int a_blendAmount, bool a_allowLights, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint)
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
    result = InitializeShader(a_Device, a_WindowHandle, vsFileName, psFileName, a_vertexShaderEntryPoint, a_pixelShaderEntryPoint);

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
                         //VertexShaderVariables.
                         XMMATRIX a_world,
                         XMMATRIX a_view,
                         XMMATRIX a_projection,
                         XMFLOAT3 a_cameraPosition,
                         float a_fogStart,
                         float a_fogEnd,
                         XMFLOAT4 a_clipPlane,
                         XMMATRIX a_reflectionMatrix,
                         XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                         //PixelShaderVariables
                         XMFLOAT4 a_lightDiffuse[NUM_LIGHTS],
                         XMFLOAT4 a_specularColor,
                         float a_SpecularPower,
                         XMFLOAT3 a_mainLightDirection,
                         XMFLOAT4 a_ambientColor,
                         XMFLOAT2 a_translationAmount,
                         float a_blendAmount,
                         float a_waterTranslation,
                         float a_reflectRefractScale,
                         XMFLOAT4 a_PixelColor,
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         ID3D11ShaderResourceView* a_Texture4,
                         ID3D11ShaderResourceView* a_Texture5)
{
    bool result;

    //set shader params that will be used for rendering
    result = SetShaderParams(a_deviceContext,
                         //VertexShaderVariables.
                         a_world,
                         a_view,
                         a_projection,
                         a_cameraPosition,
                         a_fogStart,
                         a_fogEnd,
                         a_clipPlane,
                         a_reflectionMatrix,
                         a_lightPositions,
                         //PixelShaderVariables
                         a_lightDiffuse,
                         a_specularColor,
                         a_SpecularPower,
                         a_mainLightDirection,
                         a_ambientColor,
                         a_translationAmount,
                         a_blendAmount,
                         a_waterTranslation,
                         a_reflectRefractScale,
                         a_PixelColor,
                         a_Texture1,
                         a_Texture2,
                         a_Texture3,
                         a_Texture4,
                         a_Texture5);
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
            return m_SecondaryTexture1;
        case 2:
            return m_SecondaryTexture2;
        default:
            return m_Texture;
    }
}

bool ShaderClass::InitializeShader(ID3D11Device* a_device, HWND a_windowHandle, WCHAR* a_vsFileName, WCHAR* a_psFileName, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
    
    
    //init the pointers this func will use to null
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;
    
    result = D3DCompileFromFile(a_vsFileName, NULL, NULL, a_vertexShaderEntryPoint, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
    
    result = D3DCompileFromFile(a_psFileName, NULL, NULL, a_pixelShaderEntryPoint, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
        
        
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
    int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = a_device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout);
    if (FAILED(result))
    {
        return false;
    }

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    
    
    D3D11_SAMPLER_DESC samplerDesc;
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

    //Vertex Shader Buffers
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC cameraBufferDesc;
    D3D11_BUFFER_DESC fogBufferDesc;
    D3D11_BUFFER_DESC clipPlaneBufferDesc;
    D3D11_BUFFER_DESC reflectionBufferDesc;
    D3D11_BUFFER_DESC lightPositionBufferDesc;

    //Pixel Shader Buffers
    D3D11_BUFFER_DESC lightInfoBufferDesc;
    D3D11_BUFFER_DESC translationBufferDesc;
    D3D11_BUFFER_DESC transparentBufferDesc;
    D3D11_BUFFER_DESC waterBufferDesc;
    D3D11_BUFFER_DESC pixelBufferDesc;
    
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cameraBufferDesc.MiscFlags = 0;
    cameraBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    fogBufferDesc.ByteWidth = sizeof(FogBufferType);
    fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    fogBufferDesc.MiscFlags = 0;
    fogBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&fogBufferDesc, NULL, &m_FogBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
    clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    clipPlaneBufferDesc.MiscFlags = 0;
    clipPlaneBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_ClipPlaneBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
    reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    reflectionBufferDesc.MiscFlags = 0;
    reflectionBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&reflectionBufferDesc, NULL, &m_ReflectionBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
    lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightPositionBufferDesc.MiscFlags = 0;
    lightPositionBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_LightPositionBuffer);
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
    
    translationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    translationBufferDesc.ByteWidth = sizeof(TranslationBufferType);
    translationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    translationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    translationBufferDesc.MiscFlags = 0;
    translationBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&translationBufferDesc, NULL, &m_TranslationBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    transparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    transparentBufferDesc.ByteWidth = sizeof(TransparentBufferType);
    transparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    transparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    transparentBufferDesc.MiscFlags = 0;
    transparentBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&transparentBufferDesc, NULL, &m_TransparentBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
    waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    waterBufferDesc.MiscFlags = 0;
    waterBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&waterBufferDesc, NULL, &m_WaterBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
    pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pixelBufferDesc.MiscFlags = 0;
    pixelBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&pixelBufferDesc, NULL, &m_PixelBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    return true;
}




void ShaderClass::ShutdownShader()
{
    //release all pointers
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
    
    if (m_InputLayout)
    {
        m_InputLayout->Release();
        m_InputLayout = 0;
    }
    
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
    
    if (m_CameraBuffer)
    {
        m_CameraBuffer->Release();
        m_CameraBuffer = 0;
    }
    
    if (m_FogBuffer)
    {
        m_FogBuffer->Release();
        m_FogBuffer = 0;
    }
    
    if (m_ClipPlaneBuffer)
    {
        m_ClipPlaneBuffer->Release();
        m_ClipPlaneBuffer = 0;
    }
    
    if (m_ReflectionBuffer)
    {
        m_ReflectionBuffer->Release();
        m_ReflectionBuffer = 0;
    }
    
    if (m_LightPositionBuffer)
    {
        m_LightPositionBuffer->Release();
        m_LightPositionBuffer = 0;
    }
    
    if (m_LightInformationBuffer)
    {
        m_LightInformationBuffer->Release();
        m_LightInformationBuffer = 0;
    }
    
    if (m_TranslationBuffer)
    {
        m_TranslationBuffer->Release();
        m_TranslationBuffer = 0;
    }
    
    if (m_TransparentBuffer)
    {
        m_TransparentBuffer->Release();
        m_TransparentBuffer = 0;
    }
    
    if (m_WaterBuffer)
    {
        m_WaterBuffer->Release();
        m_WaterBuffer = 0;
    }
    
    if (m_PixelBuffer)
    {
        m_PixelBuffer->Release();
        m_PixelBuffer = 0;
    }
    
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = 0;
    }
    
    if (m_SecondaryTexture1)
    {
        m_SecondaryTexture1->Release();
        m_SecondaryTexture1 = 0;
    }
    
    if (m_SecondaryTexture2)
    {
        m_SecondaryTexture2->Release();
        m_SecondaryTexture2 = 0;
    }
    
    if (m_SecondaryTexture3)
    {
        m_SecondaryTexture3->Release();
        m_SecondaryTexture3 = 0;
    }
    
    if (m_SecondaryTexture4)
    {
        m_SecondaryTexture4->Release();
        m_SecondaryTexture4 = 0;
    }
}

bool ShaderClass::SetShaderParams(ID3D11DeviceContext* a_DeviceContext,
                         //VertexShaderVariables.
                         XMMATRIX a_world,
                         XMMATRIX a_view,
                         XMMATRIX a_projection,
                         XMFLOAT3 a_cameraPosition,
                         float a_fogStart,
                         float a_fogEnd,
                         XMFLOAT4 a_clipPlane,
                         XMMATRIX a_reflectionMatrix,
                         XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                         //PixelShaderVariables
                         XMFLOAT4 a_lightDiffuse[NUM_LIGHTS],
                         XMFLOAT4 a_specularColor,
                         float a_SpecularPower,
                         XMFLOAT3 a_mainLightDirection,
                         XMFLOAT4 a_ambientColor,
                         XMFLOAT2 a_translationAmount,
                         float a_blendAmount,
                         float a_waterTranslation,
                         float reflectRefractScale,
                         XMFLOAT4 a_PixelColor,
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         ID3D11ShaderResourceView* a_Texture4,
                         ID3D11ShaderResourceView* a_Texture5)
{
    //I believe this has something to do with the register(t0) and etc. look this up later.
    a_DeviceContext->PSSetShaderResources(0,1, &a_Texture1);
    a_DeviceContext->PSSetShaderResources(1,1, &a_Texture2);
    a_DeviceContext->PSSetShaderResources(2,1, &a_Texture3);
    a_DeviceContext->PSSetShaderResources(3,1, &a_Texture4);
    a_DeviceContext->PSSetShaderResources(4,1, &a_Texture5);
    
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    int bufferNumber = 0;
    
    result = a_DeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }
    //transpose the matrices to prepare them for the shader.
    a_world = XMMatrixTranspose(a_world);
    a_view = XMMatrixTranspose(a_view);
    a_projection = XMMatrixTranspose(a_projection);
    
    //get a pointer to the data in the constant buffer
    MatrixBufferType* matrixDataPtr;
    matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;
    matrixDataPtr->world = a_world;
    matrixDataPtr->view = a_view;
    matrixDataPtr->projection = a_projection;
    
    a_DeviceContext->Unmap(m_MatrixBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    CameraBufferType* cameraDataPtr = (CameraBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_CameraBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_FogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    FogBufferType* fogDataPtr = (FogBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_FogBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_FogBuffer);
    bufferNumber++;

    result = a_DeviceContext->Map(m_ClipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    ClipPlaneBufferType* clipPlaneDataPtr = (ClipPlaneBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_ClipPlaneBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ClipPlaneBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_ReflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    ReflectionBufferType* reflectionDataPtr = (ReflectionBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_ReflectionBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ReflectionBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_LightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    LightPositionBufferType* lightPositionDataPtr = (LightPositionBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_LightPositionBuffer, 0);
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_LightPositionBuffer);
    bufferNumber++;


    //Vertex buffers above so now that we are setting pixel buffers we reset the buffer number
    bufferNumber = 0;

    
    result = a_DeviceContext->Map(m_LightInformationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    LightInformationBufferType* lightInfoDataPtr = (LightInformationBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_LightInformationBuffer, 0);
    a_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightInformationBuffer);
    bufferNumber++;

    result = a_DeviceContext->Map(m_TranslationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    TranslationBufferType* translationDataPtr = (TranslationBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_TranslationBuffer, 0);
    a_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_TranslationBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_TransparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    TransparentBufferType* transparentDataPtr = (TransparentBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_TransparentBuffer, 0);
    a_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_TransparentBuffer);
    bufferNumber++;

    result = a_DeviceContext->Map(m_WaterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    WaterBufferType* waterDataPtr = (WaterBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_WaterBuffer, 0);
    a_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_WaterBuffer);
    bufferNumber++;
    
    result = a_DeviceContext->Map(m_PixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    PixelBufferType* pixelDataPtr = (PixelBufferType*)mappedSubresource.pData;
    a_DeviceContext->Unmap(m_PixelBuffer, 0);
    a_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_PixelBuffer);
    bufferNumber++;
    
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
