#pragma once

#include "GameObject.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>

class ExitPoint
{
private:
	GameObject prop;
	
	float spinSpeed;
	float bobDifference;
	float bobSpeed;
	bool bobbingUp;

	DirectX::XMFLOAT3 centrePosition;

	float size;
public:
	ExitPoint();
	ExitPoint(GameObject obj, float spinSpeed = 1.0f, float bobDiff = 0.4f, float bobSpeed = 0.2f);
	ExitPoint(const ExitPoint& other);

	void Update(double delta);
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Frustum& frustum, Camera& cam);

	bool collidesWith(DirectX::XMFLOAT3 otherPosition, float radius);

	inline DirectX::XMFLOAT3 position() { return prop.Pos(); }
};
