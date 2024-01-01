#include "FontClass.h"

FontClass::FontClass()
{
    m_Font = 0;
    m_Texture = 0;
}

FontClass::FontClass(const FontClass& a_Copy)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* a_Device, ID3D11DeviceContext* a_Context, int a_FontChoice)
{
    char fontFileName[128];
    char fontTextureFileName[128];
    bool result;

    switch(a_FontChoice)
    {
        case 0:
        {
            strcpy_s(fontFileName, "./data/font01.txt");
            strcpy_s(fontTextureFileName, "./data/font01.tga");
            m_fontHeight = 32.0f;
            m_spaceSize = 3;    
            break;
        }
        default:
        {
            strcpy_s(fontFileName, "./data/font01.txt");
            strcpy_s(fontTextureFileName, "./data/font01.tga");
            m_fontHeight = 32.0f;
            m_spaceSize = 3;    
            break;
        }
    }

    result = LoadFontData(fontFileName);
    if (!result)
        return false;

    result = LoadTexture(a_Device, a_Context, fontTextureFileName);
    
    if (!result)
        return false;

    return true;
}

void FontClass::ShutDown()
{
    ReleaseTexture();
    ReleaseFontData();
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
    return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
    VertexType* vertexPtr;
    int numLetters, index, letter;

    //coerce the input vertices into a vertex type structure. wow the tutorial really said coerce a pointer hahahahaha
    vertexPtr = (VertexType*)vertices;
    numLetters = (int)strlen(sentence);
    index = 0;

    // Draw each letter onto a quad.
    for(int i = 0; i < numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;

        // If the letter is a space then just move over three pixels.
        if(letter == 0)
        {
            drawX = drawX + m_spaceSize;
        }
        else
        {
            // First triangle in quad.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // Bottom left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
            index++;

            // Second triangle in quad.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            // Update the x location for drawing by the size of the letter and one pixel.
            drawX = drawX + m_Font[letter].size + 1.0f;
        }
    }
}

int FontClass::GetSentencePixelLength(char* sentence)
{
    int pixelLength, numLetters, i, letter;
    pixelLength = 0;
    numLetters = (int)strlen(sentence);
    for (i = 0; i < numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;
        //if letter is space then we have a separate value to add
        if (letter == 0)
            pixelLength += m_spaceSize;
        else
            pixelLength += (m_Font[letter].size + 1);
    }
    return pixelLength;
}

int FontClass::GetFontHeight()
{
    return (int)m_fontHeight;
}

bool FontClass::LoadFontData(char* a_Filepath)
{
    ifstream fin;
    int i;
    char temp;
    
    // Create the font spacing buffer.
    m_Font = new FontType[95];
    
    // Read in the font size and spacing between chars.
    fin.open(a_Filepath);
    if(fin.fail())
    {
        return false;
    }
    
    // Read in the 95 used ascii characters for text.
    for(i=0; i<95; i++)
    {
        fin.get(temp);
        while(temp != ' ')
        {
            fin.get(temp);
        }
        fin.get(temp);
        while(temp != ' ')
        {
            fin.get(temp);
        }
    
        fin >> m_Font[i].left;
        fin >> m_Font[i].right;
        fin >> m_Font[i].size;
    }
    
    // Close the file.
    fin.close();
    
    return true;
}
    

void FontClass::ReleaseFontData()
{
    if (m_Font)
    {
        delete [] m_Font;
        m_Font = 0;
    }
}

bool FontClass::LoadTexture(ID3D11Device* a_Device, ID3D11DeviceContext* a_Context, char* a_Filename)
{
    m_Texture = new TextureClass;
    return m_Texture->Initialize(a_Device, a_Context, a_Filename);
}

void FontClass::ReleaseTexture()
{
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }
}
