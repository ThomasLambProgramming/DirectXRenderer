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
};
