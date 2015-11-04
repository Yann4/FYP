#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"

#include "data structs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Input.h"
#include "Parser.h"
#include "Skybox.h"

#include <queue>

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;

	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;

	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;

	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	
	ID3D11VertexShader*     skyboxVS;
	ID3D11PixelShader*      skyboxPS;

	Skybox skybox;

	ID3D11InputLayout*      _pVertexLayout;
	
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solid;
	bool					wfRender;

	ID3D11SamplerState* samplerLinear;

	ID3D11Buffer*           _pConstantBuffer;

	Camera camera;

	MeshData* squareMesh;

	Light light;
	GameObject go, go1, go2, go3;

	Input input;
	static std::queue<Event> inputEventQueue;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	HRESULT instantiateCube();
	HRESULT InitCubeVertexBuffer();
	HRESULT InitCubeIndexBuffer();

	void initObjects();

	static void pushEvent(Event toPush);
	void handleMessages();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

