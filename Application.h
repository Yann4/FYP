#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "resource.h"
#include "DDSTextureLoader.h"

#include "data structs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Input.h"
#include "Parser.h"
#include "Skybox.h"
#include "Frustum.h"
#include "Light.h"
#include "Collision.h"
#include "Spline.h"
#include "Surface.h"
#include "Octree.h"
#include "Graph.h"
#include "Agent.h"
#include "Player.h"

#include <queue>
#include <fstream>
#include <regex>
#include <random>
#include <thread>
#include <mutex>

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

	ID3D11VertexShader*     lineVS;
	ID3D11PixelShader*      linePS;

	ID3D11InputLayout*      _pVertexLayout;
	ID3D11InputLayout*		basicVertexLayout;
	
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solid;
	bool					wfRender;

	ID3D11SamplerState* samplerLinear;

	ID3D11Buffer*           frameConstantBuffer;
	ID3D11Buffer*           objectConstantBuffer;

	frameCB perFrameCB;

	//Camera camera;
	Frustum viewFrustum;

	MeshData* cubeMesh;
	
	std::vector<Material> houseMaterials;
	MeshData* houseMesh;

	MeshData* pipeMesh;
	std::vector<Material> pipeMaterials;

	MeshData* grassMesh;
	std::vector<Material> grassMaterial;

	MeshData* agentMesh;
	std::vector<Material> agentMaterial;

	std::vector<GameObject> objects;
	std::vector<Spline> splines;

	Player player;
	std::vector<Agent> agents;

	Skybox skybox;

	Graph navGraph;
	std::mutex graphMutex;
	bool renderGraph;
	float graphYPosition;

	Input input;
	static std::queue<Event> inputEventQueue;
	bool flashlightOn;
	float cameraMoveSpeed;
	float cameraPanSpeed;
	DirectX::XMFLOAT2 lastMousePos;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	void readInitFile(std::string fileName);
	HRESULT initialiseCube();
	HRESULT initialisePipe();
	HRESULT initialiseGrass();
	void placeCrate(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation);

	void initialiseAgents(int numAgents);

	void initObjects();

	void updateGraph(std::vector<DirectX::BoundingBox>& objects);

	void fireBox();
	static void pushEvent(Event toPush);
	void handleMessages();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	Application(const Application& other){}
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	void onMouseMove(WPARAM btnState, int x, int y);
	void Update();
	void Draw();
};

