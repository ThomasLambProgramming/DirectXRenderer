#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

public:
    XMFLOAT3 m_LightDirection;
    XMFLOAT4 m_DiffuseColor;
    XMFLOAT4 m_AmbientColor;
    XMFLOAT4 m_SpecularColor;
    float m_SpecularPower;
    //f4 just so it is a 16 to be easier to transfer to gpu.
    XMFLOAT4 m_Position;
};
