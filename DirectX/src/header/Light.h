#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Light
{
public:
    Light();
    Light(const Light&);
    ~Light();

    XMFLOAT3 m_LightDirection;
    float m_SpecularPower;
    XMFLOAT4 m_DiffuseColor;
    XMFLOAT4 m_AmbientColor;
    XMFLOAT4 m_SpecularColor;
    //f4 just so it is a 16 to be easier to transfer to gpu.
    XMFLOAT4 m_Position;
};
