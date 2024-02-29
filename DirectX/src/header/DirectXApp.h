#pragma once

//Linking #pragma comment links these libraries when using this object module
/*
 * #pragma comment(lib, lib name) tells the linker to add the 'lib name' library to the list
 * of library dependencies, as if you had added it in the project properties at
 * Linker->Input->Additional dependencies
 */
//functionality for compiling shaders.
#pragma comment(lib, "d3d11.lib")
//Contains all direct3d functionality for setup and drawing 3d graphics with dx11
#pragma comment(lib, "dxgi.lib")
//Tools to interface with the hardware on the computer, to get details on what graphics card, hz of display etc.
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class DirectXApp 
{
public:
    DirectXApp();
    DirectXApp(const DirectXApp&);
    ~DirectXApp();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();
    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX&);
    void GetWorldMatrix(XMMATRIX&);
    void GetOrthoMatrix(XMMATRIX&);

    void GetVideoCardInfo(char*, int&);
    void SetBackBufferRenderTarget();
    void ResetViewport();

    void TurnZBufferOn();
    void TurnZBufferOff();

    void EnableAlphaBlending();
    void DisableAlphaBlending();
    
private:

    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilState* m_depthDisabledStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    
    ID3D11RasterizerState* m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;

    ID3D11BlendState* m_alphaEnableBlendState;
    ID3D11BlendState* m_alphaDisableBlendState;
};