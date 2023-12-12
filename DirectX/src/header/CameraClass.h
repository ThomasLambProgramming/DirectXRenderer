#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

class CameraClass
{
public:
    CameraClass();
    CameraClass(const CameraClass& a_Copy);
    ~CameraClass();

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
#endif