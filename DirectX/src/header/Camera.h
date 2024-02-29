#pragma once 
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
    Camera();
    Camera(const Camera& a_Copy);
    ~Camera();

    void SetPosition(float x, float y, float z);
    void SetRotation(float x, float y, float z);

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void Render();
    void GetViewMatrix(XMMATRIX& a_Matrix);
private:
    float m_PositionX, m_PositionY, m_PositionZ;
	float m_RotationX, m_RotationY, m_RotationZ;
	XMMATRIX m_ViewMatrix;
};