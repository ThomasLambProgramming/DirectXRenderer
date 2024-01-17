#include "ShaderClass.h"

ShaderClass::ShaderClass()
{
    m_VertexShader = nullptr;
    m_PixelShader = nullptr;
    m_InputLayout = nullptr;
    m_SampleState = nullptr;
    m_MatrixBuffer = nullptr;
    m_LightInformationBuffer = nullptr;
    m_FogBuffer = nullptr;
    m_ClipPlaneBuffer = nullptr;
    m_ReflectionBuffer = nullptr;
    m_LightPositionBuffer = nullptr;
    m_TranslationBuffer = nullptr;
    m_TransparentBuffer = nullptr;
    m_WaterBuffer = nullptr;
    m_PixelBuffer = nullptr;
    m_SecondaryTexture1 = nullptr;
    m_SecondaryTexture2 = nullptr;
    m_SecondaryTexture3 = nullptr;
    m_SecondaryTexture4 = nullptr;
    m_CameraBuffer = nullptr;
    m_Texture = nullptr;
    m_PointLightBuffer = nullptr;
}

ShaderClass::ShaderClass(const ShaderClass& a_Copy): m_VertexShader(nullptr), m_PixelShader(nullptr),
                                                     m_InputLayout(nullptr),
                                                     m_SampleState(nullptr),
                                                     m_MatrixBuffer(nullptr),
                                                     m_CameraBuffer(nullptr),
                                                     m_FogBuffer(nullptr),
                                                     m_ClipPlaneBuffer(nullptr),
                                                     m_ReflectionBuffer(nullptr),
                                                     m_LightPositionBuffer(nullptr),
                                                     m_LightInformationBuffer(nullptr),
                                                     m_TranslationBuffer(nullptr),
                                                     m_TransparentBuffer(nullptr),
                                                     m_WaterBuffer(nullptr),
                                                     m_PixelBuffer(nullptr), m_PointLightBuffer(nullptr),
                                                     m_Texture(nullptr),
                                                     m_SecondaryTexture1(nullptr),
                                                     m_SecondaryTexture2(nullptr),
                                                     m_SecondaryTexture3(nullptr),
                                                     m_SecondaryTexture4(nullptr)
{
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle, char* a_vertexShaderEntryPoint, char* a_pixelShaderEntryPoint)
{
    wchar_t vsFileName[128];
    wchar_t psFileName[128];

    int error = wcscpy_s(vsFileName, 128, L"./src/shaders/textureVS.hlsl");
    if (error != 0)
    {
        return false;
    }
    error = wcscpy_s(psFileName, 128, L"./src/shaders/texturePS.hlsl");
    if (error != 0)
    {
        return false;
    }
    const bool result = InitializeShader(a_Device, a_WindowHandle, vsFileName, psFileName, a_vertexShaderEntryPoint, a_pixelShaderEntryPoint);

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
                const XMMATRIX& a_world,
                const XMMATRIX& a_view,
                const XMMATRIX& a_projection,
                XMFLOAT3 a_cameraPosition,
                float a_fogStart,
                float a_fogEnd,
                XMFLOAT4 a_clipPlane,
                const XMMATRIX& a_reflectionMatrix,
                XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                //PixelShaderVariables
                XMFLOAT4 a_mainLightDiffuse,
                XMFLOAT4 a_specularColor,
                float a_specularPower,
                XMFLOAT3 a_mainLightDirection,
                XMFLOAT4 a_ambientColor,
                XMFLOAT2 a_translationAmount,
                float a_blendAmount,
                float a_waterTranslation,
                float a_reflectRefractScale,
                XMFLOAT4 a_pixelColor,
                XMFLOAT4 a_pointLightDiffuse[NUM_LIGHTS],
                ID3D11ShaderResourceView* a_Texture1,
                ID3D11ShaderResourceView* a_Texture2,
                ID3D11ShaderResourceView* a_Texture3,
                ID3D11ShaderResourceView* a_Texture4,
                ID3D11ShaderResourceView* a_Texture5,
                int a_textureCount) const
{
    //set shader params that will be used for rendering
    const bool result = SetShaderParams(a_deviceContext,
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
                         a_mainLightDiffuse,
                         a_specularColor,
                         a_specularPower,
                         a_mainLightDirection,
                         a_ambientColor,
                         a_translationAmount,
                         a_blendAmount,
                         a_waterTranslation,
                         a_reflectRefractScale,
                         a_pixelColor,
                         a_pointLightDiffuse,
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

ID3D11ShaderResourceView* ShaderClass::GetTexture(int a_textureNumber) const
{
    switch(a_textureNumber)
    {
        case 0:
            return m_Texture;
        case 1:
            return m_SecondaryTexture1;
        case 2:
            return m_SecondaryTexture2;
        case 3:
            return m_SecondaryTexture3;
        case 4:
            return m_SecondaryTexture4;
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
    errorMessage = nullptr;
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer = nullptr;
    
    result = D3DCompileFromFile(a_vsFileName, nullptr, nullptr, a_vertexShaderEntryPoint, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
    
    result = D3DCompileFromFile(a_psFileName, nullptr, nullptr, a_pixelShaderEntryPoint, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
        
        
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

    result = a_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_VertexShader);
    if (FAILED(result))
    {
        return false;
    }
    
    result = a_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_PixelShader);
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
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer = nullptr;

    
    
    D3D11_SAMPLER_DESC samplerDesc;
    //Filter is the most important element of the sampler. it tells the sampler how to pick what pixels to be used or combine to create the final look of the texture on the polygon face.
    //the filter below is more expensive but gives a good(it says best but i am not sure about that) visual result. it tells the sampler to use lin-interp for minification, magnification and mip-level sampling.
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
    D3D11_BUFFER_DESC pointLightBufferDesc;
    
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&matrixBufferDesc, nullptr, &m_MatrixBuffer);
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
    result = a_device->CreateBuffer(&cameraBufferDesc, nullptr, &m_CameraBuffer);
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
    result = a_device->CreateBuffer(&lightPositionBufferDesc, nullptr, &m_LightPositionBuffer);
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
    result = a_device->CreateBuffer(&fogBufferDesc, nullptr, &m_FogBuffer);
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
    result = a_device->CreateBuffer(&clipPlaneBufferDesc, nullptr, &m_ClipPlaneBuffer);
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
    result = a_device->CreateBuffer(&reflectionBufferDesc, nullptr, &m_ReflectionBuffer);
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
    result = a_device->CreateBuffer(&lightInfoBufferDesc, nullptr, &m_LightInformationBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    pointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pointLightBufferDesc.ByteWidth = sizeof(PointLightBufferType);
    pointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pointLightBufferDesc.MiscFlags = 0;
    pointLightBufferDesc.StructureByteStride = 0;
    result = a_device->CreateBuffer(&pointLightBufferDesc, nullptr, &m_PointLightBuffer);
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
    result = a_device->CreateBuffer(&translationBufferDesc, nullptr, &m_TranslationBuffer);
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
    result = a_device->CreateBuffer(&transparentBufferDesc, nullptr, &m_TransparentBuffer);
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
    result = a_device->CreateBuffer(&waterBufferDesc, nullptr, &m_WaterBuffer);
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
    result = a_device->CreateBuffer(&pixelBufferDesc, nullptr, &m_PixelBuffer);
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
        m_VertexShader = nullptr;
    }
    
    if (m_PixelShader)
    {
        m_PixelShader->Release();
        m_PixelShader = nullptr;
    }
    
    if (m_InputLayout)
    {
        m_InputLayout->Release();
        m_InputLayout = nullptr;
    }
    
    if (m_SampleState)
    {
        m_SampleState->Release();
        m_SampleState = nullptr;
    }
    
    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = nullptr;
    }
    
    if (m_CameraBuffer)
    {
        m_CameraBuffer->Release();
        m_CameraBuffer = nullptr;
    }
    
    if (m_FogBuffer)
    {
        m_FogBuffer->Release();
        m_FogBuffer = nullptr;
    }
    
    if (m_ClipPlaneBuffer)
    {
        m_ClipPlaneBuffer->Release();
        m_ClipPlaneBuffer = nullptr;
    }
    
    if (m_ReflectionBuffer)
    {
        m_ReflectionBuffer->Release();
        m_ReflectionBuffer = nullptr;
    }
    
    if (m_LightPositionBuffer)
    {
        m_LightPositionBuffer->Release();
        m_LightPositionBuffer = nullptr;
    }
    if (m_PointLightBuffer)
    {
        m_PointLightBuffer->Release();
        m_PointLightBuffer = nullptr;
    }
    
    if (m_LightInformationBuffer)
    {
        m_LightInformationBuffer->Release();
        m_LightInformationBuffer = nullptr;
    }
    
    if (m_TranslationBuffer)
    {
        m_TranslationBuffer->Release();
        m_TranslationBuffer = nullptr;
    }
    
    if (m_TransparentBuffer)
    {
        m_TransparentBuffer->Release();
        m_TransparentBuffer = nullptr;
    }
    
    if (m_WaterBuffer)
    {
        m_WaterBuffer->Release();
        m_WaterBuffer = nullptr;
    }
    
    if (m_PixelBuffer)
    {
        m_PixelBuffer->Release();
        m_PixelBuffer = nullptr;
    }
    
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = nullptr;
    }
    
    if (m_SecondaryTexture1)
    {
        m_SecondaryTexture1->Release();
        m_SecondaryTexture1 = nullptr;
    }
    
    if (m_SecondaryTexture2)
    {
        m_SecondaryTexture2->Release();
        m_SecondaryTexture2 = nullptr;
    }
    
    if (m_SecondaryTexture3)
    {
        m_SecondaryTexture3->Release();
        m_SecondaryTexture3 = nullptr;
    }
    
    if (m_SecondaryTexture4)
    {
        m_SecondaryTexture4->Release();
        m_SecondaryTexture4 = nullptr;
    }
}

bool ShaderClass::SetShaderParams(ID3D11DeviceContext* a_deviceContext,
                         //VertexShaderVariables.
                         XMMATRIX a_world,
                         XMMATRIX a_view,
                         XMMATRIX a_projection,
                         XMFLOAT3 a_cameraPosition,
                         float a_fogStart,
                         float a_fogEnd,
                         XMFLOAT4 a_clipPlane,
                         const XMMATRIX& a_reflectionMatrix,
                         XMFLOAT4 a_lightPositions[NUM_LIGHTS],
                         //PixelShaderVariables
                         XMFLOAT4 a_mainLightDiffuse,
                         XMFLOAT4 a_specularColor,
                         float a_specularPower,
                         XMFLOAT3 a_mainLightDirection,
                         XMFLOAT4 a_ambientColor,
                         XMFLOAT2 a_translationAmount,
                         float a_blendAmount,
                         float a_waterTranslation,
                         float a_reflectRefractScale,
                         XMFLOAT4 a_pixelColor,
                         XMFLOAT4 a_pointLightDiffuse[NUM_LIGHTS],
                         ID3D11ShaderResourceView* a_Texture1,
                         ID3D11ShaderResourceView* a_Texture2,
                         ID3D11ShaderResourceView* a_Texture3,
                         ID3D11ShaderResourceView* a_Texture4,
                         ID3D11ShaderResourceView* a_Texture5,
                         int a_textureCount) const
{
    //I believe this has something to do with the register(t0) and etc. look this up later.
    a_deviceContext->PSSetShaderResources(0,1, &a_Texture1);
    if (a_textureCount > 1)
        a_deviceContext->PSSetShaderResources(1,1, &a_Texture2);
    if (a_textureCount > 2)
        a_deviceContext->PSSetShaderResources(2,1, &a_Texture3);
    if (a_textureCount > 3)
        a_deviceContext->PSSetShaderResources(3,1, &a_Texture4);
    if (a_textureCount > 4)
        a_deviceContext->PSSetShaderResources(4,1, &a_Texture5);
    
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    int bufferNumber = 0;
    
    HRESULT result = a_deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }
    //transpose the matrices to prepare them for the shader.
    a_world = XMMatrixTranspose(a_world);
    a_view = XMMatrixTranspose(a_view);
    a_projection = XMMatrixTranspose(a_projection);
    
    //get a pointer to the data in the constant buffer
    MatrixBufferType* matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;
    matrixDataPtr->world = a_world;
    matrixDataPtr->view = a_view;
    matrixDataPtr->projection = a_projection;
    
    a_deviceContext->Unmap(m_MatrixBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    CameraBufferType* cameraDataPtr = (CameraBufferType*)mappedSubresource.pData;
    cameraDataPtr->padding = 0.0f;
    cameraDataPtr->cameraPosition = a_cameraPosition;
    a_deviceContext->Unmap(m_CameraBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);
    bufferNumber++;

    result = a_deviceContext->Map(m_LightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    LightPositionBufferType* lightPositionDataPtr = (LightPositionBufferType*)mappedSubresource.pData;
    lightPositionDataPtr->lightPosition[0] = a_lightPositions[0]; 
    lightPositionDataPtr->lightPosition[1] = a_lightPositions[1]; 
    lightPositionDataPtr->lightPosition[2] = a_lightPositions[2]; 
    lightPositionDataPtr->lightPosition[3] = a_lightPositions[3]; 
        
    a_deviceContext->Unmap(m_LightPositionBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_LightPositionBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_FogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    FogBufferType* fogDataPtr = (FogBufferType*)mappedSubresource.pData;
    fogDataPtr->fogStart = a_fogStart;
    fogDataPtr->fogEnd = a_fogEnd;
    fogDataPtr->fogPadding = XMFLOAT2(0.0f,0.0f);
    a_deviceContext->Unmap(m_FogBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_FogBuffer);
    bufferNumber++;

    result = a_deviceContext->Map(m_ClipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    ClipPlaneBufferType* clipPlaneDataPtr = (ClipPlaneBufferType*)mappedSubresource.pData;
    clipPlaneDataPtr->clipPlane = a_clipPlane;
    a_deviceContext->Unmap(m_ClipPlaneBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ClipPlaneBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_ReflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    ReflectionBufferType* reflectionDataPtr = (ReflectionBufferType*)mappedSubresource.pData;
    reflectionDataPtr->reflectionMatrix = a_reflectionMatrix;
    a_deviceContext->Unmap(m_ReflectionBuffer, 0);
    a_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ReflectionBuffer);
    //bufferNumber++;

    
    //Vertex buffers above so now that we are setting pixel buffers we reset the buffer number
    bufferNumber = 0;

    
    result = a_deviceContext->Map(m_LightInformationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    LightInformationBufferType* lightInfoDataPtr = (LightInformationBufferType*)mappedSubresource.pData;
    lightInfoDataPtr->diffuseColor = a_mainLightDiffuse;
    lightInfoDataPtr->ambientColor = a_ambientColor;
    lightInfoDataPtr->specularColor = a_specularColor;
    lightInfoDataPtr->mainLightDirection = a_mainLightDirection;
    lightInfoDataPtr->specularPower = a_specularPower;
    a_deviceContext->Unmap(m_LightInformationBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightInformationBuffer);
    bufferNumber++;

    result = a_deviceContext->Map(m_PointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    PointLightBufferType* pointLightDataPtr = (PointLightBufferType*)mappedSubresource.pData;
    for (int i = 0; i < NUM_LIGHTS; i++)
        pointLightDataPtr->pointLightDiffuseColor[i] = a_pointLightDiffuse[i];
    a_deviceContext->Unmap(m_PointLightBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_PointLightBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_TranslationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    TranslationBufferType* translationDataPtr = (TranslationBufferType*)mappedSubresource.pData;
    translationDataPtr->textureTranslation = a_translationAmount;
    translationDataPtr->textureTranslationPadding = XMFLOAT2(0.0f,0.0f);
    a_deviceContext->Unmap(m_TranslationBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_TranslationBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_TransparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    TransparentBufferType* transparentDataPtr = (TransparentBufferType*)mappedSubresource.pData;
    transparentDataPtr->blendAmount = a_blendAmount;
    transparentDataPtr->blendPadding = XMFLOAT3(0.0f,0.0f,0.0f);
    a_deviceContext->Unmap(m_TransparentBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_TransparentBuffer);
    bufferNumber++;

    result = a_deviceContext->Map(m_WaterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    WaterBufferType* waterDataPtr = (WaterBufferType*)mappedSubresource.pData;
    waterDataPtr->waterPadding = XMFLOAT2(0,0);
    waterDataPtr->waterTranslation = a_waterTranslation;
    waterDataPtr->reflectRefractScale = a_reflectRefractScale;
    a_deviceContext->Unmap(m_WaterBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_WaterBuffer);
    bufferNumber++;
    
    result = a_deviceContext->Map(m_PixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result))
        return false;
    PixelBufferType* pixelDataPtr = (PixelBufferType*)mappedSubresource.pData;
    pixelDataPtr->pixelColor = a_pixelColor;
    a_deviceContext->Unmap(m_PixelBuffer, 0);
    a_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_PixelBuffer);
    //bufferNumber++;
    
    return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount) const
{
    //set the vertex input layout
    a_DeviceContext->IASetInputLayout(m_InputLayout);
    a_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    a_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
    a_DeviceContext->PSSetSamplers(0, 1, &m_SampleState);
    a_DeviceContext->DrawIndexed(a_IndexCount, 0, 0);
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, const WCHAR* a_FilePath)
{
    ofstream inOutStream;

    //get pointer to the error message text buffer
    const char* compileErrors = (char*)(a_ErrorMessage->GetBufferPointer());

    //get length of the message
    const unsigned long long bufferSize = a_ErrorMessage->GetBufferSize();

    //open a file to write the error message to.
    inOutStream.open("ShaderError.txt");
    for (int i = 0; i < bufferSize; i++)
    {
        inOutStream << compileErrors[i];
    }
    inOutStream.close();

    a_ErrorMessage->Release();
    a_ErrorMessage = nullptr;

    MessageBox(a_WindowHandle, L"Error compiling shader. Check ShaderError.txt for message.", a_FilePath, MB_OK);
}
