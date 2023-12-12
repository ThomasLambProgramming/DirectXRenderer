#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
    m_VertexShader = 0;
    m_PixelShader = 0;
    m_Layout = 0;
    m_MatrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& copy)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* a_Device, HWND a_WindowHandle)
{
    bool result;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    int error;
    
    //Set name of vertex and pixel shader
    error = wcscpy_s(vsFileName, 128, L"./src/shaders/colorVS.hlsl");
    if (error != 0)
    {
        return false;
    }

    error = wcscpy_s(psFileName, 128, L"./src/shaders/colorPS.hlsl");
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

void ColorShaderClass::Shutdown()
{
    ShutdownShader();
    return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount,
    XMMATRIX a_World, XMMATRIX a_View, XMMATRIX a_Projection)
{
    bool result;
    //set shader params for rendering
    result = SetShaderParameters(a_DeviceContext, a_World, a_View, a_Projection);
    if (!result)
    {
        return false;
    }
    RenderShader(a_DeviceContext, a_IndexCount);
    return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* a_Device, HWND a_WindowHandle, WCHAR* vsFileName, WCHAR* psFileName)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    //init pointers to null
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    result = D3DCompileFromFile(
        vsFileName,
        NULL,
        NULL,
        "ColorVertexShader",
        "vs_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS,
        0,
        &vertexShaderBuffer,
        &errorMessage);

    if (FAILED(result))
    {
        //shader failed to compile so now we check the error message
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, a_WindowHandle, vsFileName);
        }
        //if no error message then the file was not found
        else
        {
            MessageBox(a_WindowHandle, vsFileName, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    //now compile the color / pixel shader
    result = D3DCompileFromFile(
        psFileName,
        NULL,
        NULL,
        "ColorPixelShader",
        "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS,
        0,
        &pixelShaderBuffer,
        &errorMessage);

    if (FAILED(result))
    {
        //shader failed to compile so now we check the error message
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, a_WindowHandle, psFileName);
        }
        //if no error message then the file was not found
        else
        {
            MessageBox(a_WindowHandle, psFileName, L"Missing Shader File", MB_OK);
        }
        return false;
    }


    result = a_Device->CreateVertexShader(
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        NULL,
        &m_VertexShader);

    if (FAILED(result))
    {
        return false;
    }
    
    result = a_Device->CreatePixelShader(
        pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        NULL,
        &m_PixelShader);

    if (FAILED(result))
    {
        return false;
    }

    //Create the vertex input layout description
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = a_Device->CreateInputLayout(polygonLayout,
                                         numElements,
                                         vertexShaderBuffer->GetBufferPointer(),
                                         vertexShaderBuffer->GetBufferSize(),
                                         &m_Layout);
    if (FAILED(result))
    {
        return false;
    }

    //Release the shader buffers since we dont need them anymore
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    result = a_Device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);

    if (FAILED(result))
    {
        return false;
    }

    if (!vertexShaderBuffer)
    {
        vertexShaderBuffer = 0;
    }
    
    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = 0;
    }
    if (m_Layout)
    {
        m_Layout->Release();
        m_Layout = 0;
    }
    if (m_PixelShader)
    {
        m_PixelShader->Release();
        m_PixelShader = 0;
    }
    if (m_VertexShader)
    {
        m_VertexShader->Release();
        m_VertexShader = 0;
    }
    return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* a_ErrorMessage, HWND a_WindowHandle, WCHAR* shaderFileName)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;
    //get pointer to error message text buffer
    compileErrors = (char*)(a_ErrorMessage->GetBufferPointer());
    //get length of message
    bufferSize = a_ErrorMessage->GetBufferSize();
    fout.open("ShaderError.txt");
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }
    fout.close();
    //release error message
    a_ErrorMessage->Release();
    a_ErrorMessage = 0;

    //make a pop up to notify the error
    MessageBox(a_WindowHandle, L"The shader failed to compile, please check ShaderError.txt for error log.", shaderFileName, MB_OK);
    return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* a_DeviceContext,
    XMMATRIX a_World,
    XMMATRIX a_View,
    XMMATRIX a_Projection)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    //Transpose matrices to prepare them for the shader
    //(its a requirement from dx11??? (XMMATRIX is row order, hlsl is Column order.
    //why the fuck. THEY ARE BOTH MICROSOFT AAAAAAAAHHHHHHHHHHHHHHH im leaving this here to show my pain.)
    a_World = XMMatrixTranspose(a_World );
    a_View = XMMatrixTranspose(a_View);
    a_Projection = XMMatrixTranspose(a_Projection);

    //lock the constant buffer so it can be written to. (gets a pointer to the data contained in a subresources and denies the gpu access to the subresource)
    //textures can be considered one subresource and it seems most straight 1d things will be 1 subresouce but be aware it could be more.
    result = a_DeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }
    //get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    //copy the matrices into the constant buffer.
    dataPtr->world = a_World;
    dataPtr->view = a_View;
    dataPtr->projection = a_Projection;

    //unlock the constant buffer
    a_DeviceContext->Unmap(m_MatrixBuffer, 0);

    //now set the updated matrix buffer in hlsl vert shader
    //set the position of the constant buffer in the vert shader
    bufferNumber = 0;
    //Finally set the constant buffer int he vert shader with the updated values
    //man its confusing/ annoying how little people explain with buffers, locking, how the data is sent and etc.
    a_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* a_DeviceContext, int a_IndexCount)
{
    //set the vertex input layout
    a_DeviceContext->IASetInputLayout(m_Layout);
     //Set the vertex and pixel shaders that is used to render a triangle.
    a_DeviceContext->VSSetShader(m_VertexShader, NULL, 0);
    a_DeviceContext->PSSetShader(m_PixelShader, NULL, 0);

    //Render triangle.
    a_DeviceContext->DrawIndexed(a_IndexCount, 0,0);
    return;
}
