#include "TextureClass.h"

TextureClass::TextureClass()
{
    m_Texture = nullptr;
    m_TargaData = nullptr;
    m_TextureView = nullptr;
}

TextureClass::TextureClass(const TextureClass& a_Copy)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_DeviceContext, char* a_FileName)
{
    bool result;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT hResult;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    //load the targe image into memory;
    result = LoadTarga32Bit(a_FileName);
    if (!result)
    {
        return false;
    }

    textureDesc.Height = m_Height;
    textureDesc.Width = m_Width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    //Create empty texture
    hResult = a_Device->CreateTexture2D(&textureDesc, nullptr, &m_Texture);
    if (FAILED(hResult))
    {
        return false;
    }

    //I believe this would be rgba * width in pixels * size of each 256 possible int value.
    //so it knows when to check for the next row.
    rowPitch = (m_Width * 4) * sizeof(unsigned char);

    //copy targa information into the texture.
    a_DeviceContext->UpdateSubresource(m_Texture, 0, nullptr, m_TargaData, rowPitch, 0);

    //create a shader resource view which allows us to have a pointer to set the texture in shaders.
    //we set to important mipmap variables which will give us the full range of mipmap levels for high quality texture rendering
    //at any distance after view is made we call generatemips and it will create mipmaps for us. we can load our own to get better quality.
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1; //miplevels is unsigned so its just uint.max.

    hResult = a_Device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureView);
    if (FAILED(hResult))
    {
        return false;
    }
    // Generate mipmaps for the texture
    a_DeviceContext->GenerateMips(m_TextureView);
    delete [] m_TargaData;
    m_TargaData = nullptr;
    return true;
}

void TextureClass::Shutdown()
{
    if (m_TextureView)
    {
        m_TextureView->Release();
        m_TextureView = nullptr;
    }
    if (m_TargaData)
    {
        delete [] m_TargaData;
        m_TargaData = nullptr;
    }
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = nullptr;
    }
    return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_TextureView;
}

int TextureClass::GetWidth()
{
    return m_Width;
}

int TextureClass::GetHeight()
{
    return m_Height;
}

bool TextureClass::LoadTarga32Bit(char* a_FileName)
{
    int error, bpp, imagesize, index, i,j,k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;

    error = fopen_s(&filePtr, a_FileName, "rb");
    if (error != 0)
    {
        return false;
    }
    //read in the file header. (i think this reads in one element to check)
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    if (count != 1)
    {
        return false;
    }
    //get the important info from the header
    m_Height = (int)targaFileHeader.height;
    m_Width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;

    //check that it is 32 bit and not 24bit
    if (bpp != 32)
    {
        return false;
    }
    //calulate the size of the 32 bit image data.
    imagesize = m_Width * m_Height * 4;
    //allocate memory for the image data.
    targaImage = new unsigned char[imagesize];

    //read in the targa image data.
    count = (unsigned int)fread(targaImage, 1, imagesize, filePtr);
    if (count != imagesize)
    {
        return false;
    }
    //close file.
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }
    
    //allocate memory for the targa destination data.
    m_TargaData = new unsigned char[imagesize];
    index = 0;

    //init the index into the targa image data.
    //I dont understand this bit that well. but we read in the data. and need to flip it since its stored the wrong way around for
    //what we are using it for. and its not in rgba format 0,1,2,3 so we do that as well.
    k = (m_Width * m_Height * 4) - (m_Width * 4);
    for (j = 0; j < m_Height; j++)
    {
        for (i = 0; i < m_Width; i++)
        {
            m_TargaData[index + 0] = targaImage[k + 2]; // red
            m_TargaData[index + 1] = targaImage[k + 1]; // green
            m_TargaData[index + 2] = targaImage[k + 0]; // blue
            m_TargaData[index + 3] = targaImage[k + 3]; // alpha

            //increment the indexes into the targa data.
            k += 4;
            index += 4;
        }

        //set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down
        k -= (m_Width * 8);
    }

    //release targa image data now that it has been copied.
    delete [] targaImage;
    targaImage = nullptr;
    return true;
    //I will need to look at reading that in + other format file reading + c style file io since i have never seen that before.
}
