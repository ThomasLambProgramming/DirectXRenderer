#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct Light
{
    XMFLOAT4 m_Position;
    XMFLOAT4 m_DiffuseColor;
    XMFLOAT4 m_SpecularColor;
    float m_SpecularPower;
    XMFLOAT3 m_LightDirection;
    XMFLOAT4 m_AmbientColor;
};
