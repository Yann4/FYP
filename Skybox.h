#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "DDSTextureLoader.h"
#include "data structs.h"
#include "Camera.h"

#include <vector>
#include <string>

class Skybox
{
private:
	struct skyConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX projection;
	};

	ID3D11DeviceContext* _pImmediateContext;
	ID3D11Buffer* _pConstantBuffer;

	ID3D11Buffer* sphereIBuff;
	ID3D11Buffer* sphereVBuff;

	ID3D11ShaderResourceView* skySRV;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;
	ID3D11SamplerState* CubesTexSamplerState;

	int noSphereVert, noSphereFace;
	DirectX::XMMATRIX sphereWorld;

	void CreateSphere(ID3D11Device* _pd3dDevice, int latLines, int longLines);

public:
	Skybox();
	~Skybox();

	void init(ID3D11DeviceContext* context, ID3D11Device* _pd3dDevice, LPCTSTR fileName);
	void Update(Camera* cam);
	void Draw(ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, Camera* cam);
};