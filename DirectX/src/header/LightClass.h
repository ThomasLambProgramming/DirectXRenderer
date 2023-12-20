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
};
