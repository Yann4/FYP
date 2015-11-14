#include "Application.h"

using namespace DirectX;

//This is a static variable that's been declared in Application.h
std::queue<Event> Application::inputEventQueue;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	squareMesh = new MeshData();
	_pConstantBuffer = nullptr;
	samplerLinear = nullptr;
	_depthStencilBuffer = nullptr;
	_depthStencilView = nullptr;
	_solid = nullptr;
	samplerLinear = nullptr;

	cameraMoveSpeed = 0.1f;
	cameraPanSpeed = 0.25f;
	lastMousePos = XMFLOAT2(0, 0);
	objects = std::vector<GameObject>();
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	camera = Camera(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f);

	input = Input("input_map.txt");

	viewFrustum = Frustum();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

	ID3DBlob* skyboxVSBuffer = nullptr;
	ID3DBlob* skyboxPSBuffer = nullptr;

	//Skybox
	hr = CompileShaderFromFile(L"SkyBox.fx", "SKYMAP_VS", "vs_4_0", &skyboxVSBuffer);
	hr = CompileShaderFromFile(L"SkyBox.fx", "SKYMAP_PS", "ps_4_0", &skyboxPSBuffer);

	hr = _pd3dDevice->CreateVertexShader(skyboxVSBuffer->GetBufferPointer(), skyboxVSBuffer->GetBufferSize(), NULL, &skyboxVS);
	hr = _pd3dDevice->CreatePixelShader(skyboxPSBuffer->GetBufferPointer(), skyboxPSBuffer->GetBufferSize(), NULL, &skyboxPS);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::initialiseCube()
{
	squareMesh->material.diffuse = XMFLOAT4(0.8f, 0.7f, 0.5f, 1.0f);
	squareMesh->material.specular = XMFLOAT4(0.4f, 0.35f, 0.25f, 1.0f);
	squareMesh->material.ambient = XMFLOAT4(0.4, 0.35, 0.25, 0.2);

	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &(squareMesh->textureRV));
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_SPEC.dds", nullptr, &(squareMesh->specularRV));
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_NRM.dds", nullptr, &(squareMesh->normalMapRV));

	Parser p;
	p.readObjFile(_pd3dDevice, "cube.txt", squareMesh, nullptr);
	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
		if (pErrorBlob != nullptr)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC ds_desc;
	ds_desc.Width = _WindowWidth;
	ds_desc.Height = _WindowHeight;
	ds_desc.MipLevels = 1;
	ds_desc.ArraySize = 1;
	ds_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ds_desc.SampleDesc.Count = 1;
	ds_desc.SampleDesc.Quality = 0;
	ds_desc.Usage = D3D11_USAGE_DEFAULT;
	ds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ds_desc.CPUAccessFlags = 0;
	ds_desc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&ds_desc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	D3D11_RASTERIZER_DESC wf;
	ZeroMemory(&wf, sizeof(D3D11_RASTERIZER_DESC));
	wf.FillMode = D3D11_FILL_WIREFRAME;
	wf.CullMode = D3D11_CULL_NONE;
	_pd3dDevice->CreateRasterizerState(&wf, &_wireFrame);

	D3D11_RASTERIZER_DESC sol;
	ZeroMemory(&sol, sizeof(D3D11_RASTERIZER_DESC));
	sol.FillMode = D3D11_FILL_SOLID;
	sol.CullMode = D3D11_CULL_NONE;
	sol.MultisampleEnable = true;
	_pd3dDevice->CreateRasterizerState(&sol, &_solid);

	_pImmediateContext->RSSetState(_solid);
	wfRender = false;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &samplerLinear);
	_pImmediateContext->PSSetSamplers(0, 1, &samplerLinear);

	initObjects();

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::readInitFile(std::string fileName)
{
	/*
	Reads world data from file.
	The syntax for the file is:
	Lines starting with a # is treated as a comment
	Other than that, each line should be an isolated piece of information that gives all of the data needed to instantiate the object in the format:
	MESHNAME LOCATION SCALE ROTATION
	e.g. CRATE (0,0,0) (1,1,1) (0,0,0)

	The list of valid meshnames are:
	CRATE
	*/

	std::fstream worldFile;
	worldFile.open(fileName, std::fstream::in);

	if (!worldFile.is_open())
	{
		throw std::exception::exception("Error opening file");
	}

	std::string line;
	std::regex const matcher("([[:upper:]]+) \\(([[:digit:]]+),([[:digit:]]+),([[:digit:]]+)\\) \\(([[:digit:]]+),([[:digit:]]+),([[:digit:]]+)\\) \\(([[:digit:]]+),([[:digit:]]+),([[:digit:]]+)\\)");

	while (std::getline(worldFile, line))
	{
		std::smatch captures;
		if (line.find('#') != std::string::npos)
		{
			continue;
		}

		if (std::regex_match(line, captures, matcher))
		{
			std::string name = captures[1];
			XMFLOAT3 position(std::stof(captures[2]), std::stof(captures[3]), std::stof(captures[4]));
			XMFLOAT3 scale(std::stof(captures[5]), std::stof(captures[6]), std::stof(captures[7]));
			XMFLOAT3 rotation(std::stof(captures[8]), std::stof(captures[9]), std::stof(captures[10]));

			if (name == "CRATE")
			{
				GameObject temp = GameObject(_pImmediateContext, _pConstantBuffer, squareMesh, position);
				temp.setScale(scale.x, scale.y, scale.z);
				temp.setRotation(rotation.x, rotation.y, rotation.z);
				temp.UpdateMatrix();
				objects.push_back(temp);
			}
		}
	}
}

void Application::initObjects()
{
	initialiseCube();

	light = Light(XMFLOAT4(0.2, 0.2, 0.2, 1.0), XMFLOAT4(0.9f, 0.9f, 0.9f, 0.9f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, XMFLOAT3(0.25f, 0.5f, -1.0f));

	readInitFile("worldData.txt");
	skybox = Skybox();
	skybox.init(_pImmediateContext, _pd3dDevice, L"snowcube.dds");
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solid) _solid->Release();
	if (skyboxPS) skyboxPS->Release();
	if (skyboxVS) skyboxVS->Release();

	delete squareMesh;
}

void Application::onMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(cameraPanSpeed * (x - lastMousePos.x));
		float dy = XMConvertToRadians(cameraPanSpeed * (y - lastMousePos.y));

		camera.Pitch(dy);
		camera.Yaw(dx);
	}

	lastMousePos.x = x;
	lastMousePos.y = y;
	camera.Update();
}

void Application::pushEvent(Event toPush)
{
	inputEventQueue.push(toPush);
}

void Application::handleMessages()
{
	while (!inputEventQueue.empty())
	{
		Event next = inputEventQueue.front();

		switch (next)
		{
		case WALK_FORWARDS:
			camera.Walk(0.001f);
			break;
		case WALK_BACKWARDS:
			camera.Walk(-0.001f);
			break;
		case STRAFE_LEFT:
			camera.Strafe(-0.001f);
			break;
		case STRAFE_RIGHT:
			camera.Strafe(0.001f);
			break;
		case YAW_LEFT:
			camera.Yaw(-0.001f);
			break;
		case YAW_RIGHT:
			camera.Yaw(0.001f);
			break;
		case PITCH_UP:
			camera.Pitch(-0.001f);
			break;
		case PITCH_DOWN:
			camera.Pitch(0.001f);
			break;
		case TOGGLE_WIREFRAME:
			if (!wfRender)
			{
				_pImmediateContext->RSSetState(_wireFrame);
				wfRender = true;
			}
			else
			{
				_pImmediateContext->RSSetState(_solid);
				wfRender = false;
			}
		case NO_SUCH_EVENT:
		default:
			break;
		}
		inputEventQueue.pop();
	}
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG dwTimeStart = 0;
        ULONGLONG dwTimeCur = GetTickCount64();

		if (dwTimeStart == 0)
		{
			dwTimeStart = dwTimeCur;
		}

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	input.handleInput(&Application::pushEvent);
	handleMessages();
	camera.Update();
	skybox.Update(&camera);
	for (GameObject object : objects)
	{
		object.Update(t);
	}
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	viewFrustum.constructFrustum(camera.viewDistance(), camera.getProjection(), camera.getView());
	for (GameObject object : objects)
	{
		object.Draw(_pPixelShader, _pVertexShader, viewFrustum, camera, light);
	}

	skybox.Draw(skyboxVS, skyboxPS, &camera);
    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}