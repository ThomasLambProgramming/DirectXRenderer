#include "Direct3DClass.h"
#include <fstream>
#include <iostream>
#include <vector>

Direct3DClass::Direct3DClass()
{
	//Set all pointers to 0;
    m_swapChain = nullptr;
    m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
	m_depthDisabledStencilState = nullptr;
	m_alphaEnableBlendState = nullptr;
	m_alphaDisableBlendState = nullptr;
}

Direct3DClass::Direct3DClass(const Direct3DClass&)
{
}

Direct3DClass::~Direct3DClass()
{
}

bool Direct3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	//hresult is an int, but bits are split into flags, the code result, and severity etc.
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;
	D3D11_BLEND_DESC blendStateDescription;
	//Store the vsync setting
	m_vsync_enabled = vsync;

	//Create a directx graphics interface factory (__uuidof returns expressions GUID globally unique identifier)
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//This will log all adapters (video cards) that are avaliable to the system.
	//std::ofstream AdapterFile = std::ofstream("AdapterFile.txt");
	//std::vector<std::string> adapterList = std::vector<std::string>();
	//DXGI_ADAPTER_DESC adapterOutputForTesting;
	//IDXGIAdapter* checkAdapter;
	//for (int adapterCheck = 0; SUCCEEDED(factory->EnumAdapters(adapterCheck, &checkAdapter)); adapterCheck++)
	//{
	//	checkAdapter->GetDesc(&adapterOutputForTesting);
	//	std::wstring ws = std::wstring(adapterOutputForTesting.Description);
	//	adapterList.push_back(std::string(ws.begin(), ws.end()) + "\n");
	//}
	//if (!adapterList.empty())
	//{
	//	for (int j = 0; j < adapterList.size(); j++)
	//	{
	//		AdapterFile << adapterList[j];
	//	}
	//	AdapterFile.close();
	//}

	
	//use factory to create an adapter for the primary graphics interface (video card)
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}
	
	//Enumerate the primary adapter output(monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}
	
	//get number of modes that fit DXGI_FORMAT_R8G8B8A*_UNORM display format for the adapterOutput (monitor)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}
	
	//create list to hold all possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}
	
	//now fill the display mode list structure
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}
	
	// now go through all the display modes and find the one that matches the screen width and height.
	//when a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//get the adapter (video card) description
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}
	
	//store the dedicated video card memory in megabytes
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//conver the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}
	
	//release display mode list
	delete [] displayModeList;
	displayModeList = nullptr;

	//release adapter output
	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;
	
	factory->Release();
	factory = nullptr;

	//Init the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set to a single back buffer
	swapChainDesc.BufferCount = 1;
	//set the width and height of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//set regular 32-bit surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//set the refresh rate of the back buffer
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//set the useage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	//turn multisampling off. count=num multisamples per pixel.
	swapChainDesc.SampleDesc.Count = 1;
	//quality is between 0 and 1, the image quality level. dont really know what this does yet.
	swapChainDesc.SampleDesc.Quality = 0;

	//set to full screen or windows mode
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	//set the scan line ordering and scaling to unspecified, flags indicating the method the raster uses to create an image on a surface
	//we just set it to not have an order, im assuming because keeping it in order causes performance issues.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//scaling is how the image is stretched to fit an image according to the monitors resolution, we specify it doesnt.
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//dont set the advanced flags
	swapChainDesc.Flags = 0;

	//set the feature level to direct x 11, this is really only for targeting specific hardware or potatoes, otherwise we do dx11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//In Direct3D 11, a device is separated into a device object for creating resources and a device-context object, which performs rendering.
	//Direct3D Object = creates and destroys resources(objects) and context which does rendering. (in other dx versions they were one object).
	//Create the swap chain, Direct3D device and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, 
	                                       D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

	if (FAILED(result))
	{
		return false;
	}

	//If the swap chain's swap effect is DXGI_SWAP_EFFECT_DISCARD, this method can only access the first buffer; for this situation, set the index to zero.
	//get pointer back to the buffer, for the most part we just do 0 unless there is some specific magic being done (so for me never pretty much).
	//Dont really get the __uuidof(type), (LPVOID*)&backBufferPtr) part but it gets the buffer in the form of a texture2d i think?
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//When rendering in Direct3D, you must establish the render target.
	//This is a simple COM object that maintains a location in video memory for you to render into.
	//In most cases (including our case) this is the back buffer.
	//create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	//rendertarget view is a framebuffer, its a texture we can bind to read/write to.
	if (FAILED(result))
	{
		return false;
	}

	//Back buffer= * to texture, rendertargetview is address of the render target inferface so im assuming stuff happens to interface with render target. but we affect the back buffer.
	
	
	//release pointer to the back buffer as we no longer need it. 
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//init the description of the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//Set up the description of the depth buffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Init the description of the stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Set up the description of the stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF; //1111 1111 (so all)
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}
	//Set the depth stencil state
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//Init the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//setup the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // just means it will be accessed as a 2d texture. fml dx has horrid naming.
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//create the depth stencil view
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//these are our render targets. (ooga booga the big important words)
	//Bind the render target view and depth stencil buffer to the output render pipeline
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	//setup the viewport for rendering.
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	

	//Create viewport
	m_deviceContext->RSSetViewports(1, &m_viewport);

	//setup projectionMatrix
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//XMMatrixPerspectiveFovLH = Left handed projection matrix function.
	//create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	m_worldMatrix = XMMatrixIdentity();
	//create an orthographic projection matrix for 2D rendering, this allows us to skip the 3d rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	//Clear the seocnd depth stencil state before setting parameters;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	//make the two blending states, currently just used for blending the text background so the small black triangles
	//will be overwritten through blending otherwise we would just have the text basically having a background.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendState);
	if (FAILED(result))
	{
		return false;
	}
	
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendState);
	if (FAILED(result))
	{
		return false;
	}
	
	return true;
	
}

void Direct3DClass::Shutdown()
{
	//before shutting down set to windowed mode or when you relase the swap chain it will throw an exception
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}
	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	if (m_alphaDisableBlendState)
	{
		m_alphaDisableBlendState->Release();
		m_alphaDisableBlendState = nullptr;
	}
	if (m_alphaEnableBlendState)
	{
		m_alphaEnableBlendState->Release();
		m_alphaEnableBlendState = nullptr;
	}
	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}
	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = nullptr;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
	return;
}

void Direct3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	//setup color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//clear depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void Direct3DClass::EndScene()
{
	//present the back buffer to the screen since rendering is complete
	if (m_vsync_enabled)
	{
		//lock to screen refresh rate
		m_swapChain->Present(1,0);
	}
	else
	{
		//present as fast as possible
		m_swapChain->Present(0,0);
	}
	return;
}

ID3D11Device* Direct3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* Direct3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void Direct3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void Direct3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void Direct3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void Direct3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void Direct3DClass::SetBackBufferRenderTarget()
{
	//bind the render target view and depth stencil buffer to the output render pipeline,
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return;
}

void Direct3DClass::ResetViewport()
{
	//set the viewport
	m_deviceContext->RSSetViewports(1, &m_viewport);
	return;
}

void Direct3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void Direct3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void Direct3DClass::EnableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendState, blendFactor, 0xffffffff);
}

void Direct3DClass::DisableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendState, blendFactor, 0xffffffff);
}
