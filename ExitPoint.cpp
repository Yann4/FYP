#include "ExitPoint.h"

using namespace DirectX;

ExitPoint::ExitPoint()
{
	prop = GameObject();
	spinSpeed = 0.5f;
	bobDifference = 1.0f;
	bobSpeed = 0.5f;
	bobbingUp = true;
	centrePosition = XMFLOAT3(0, 0, 0);
	size = 0.0f;
}

ExitPoint::ExitPoint(GameObject obj, float spinSpeed, float bobDiff, float bobSpeed): prop(obj), spinSpeed(spinSpeed), bobDifference(bobDiff), bobSpeed(bobSpeed)
{
	centrePosition = obj.Pos();
	obj.setOnGround(true);
	bobbingUp = true;
	size = fmaxf(obj.Size().x, obj.Size().z);
}

ExitPoint::ExitPoint(const ExitPoint& other)
{
	prop = other.prop;
	spinSpeed = other.spinSpeed;
	bobDifference = other.bobDifference;
	bobSpeed = other.bobSpeed;
	bobbingUp = other.bobbingUp;
	centrePosition = other.centrePosition;
	size = other.size;
}

void ExitPoint::Update(double delta)
{
	XMFLOAT3 rot = prop.Rotation();
	double spinThisFrame = (XM_2PI * spinSpeed) * delta;

	prop.setRotation(rot.x, spinThisFrame, rot.z);

	if (bobbingUp)
	{
		prop.setTranslation(0, bobSpeed * (float)delta, 0);
	}
	else
	{
		prop.setTranslation(0, -bobSpeed * (float)delta, 0);
	}

	prop.Update(delta);

	//Check if we should change bob direction
	float yPos = prop.Pos().y;
	float centreYPos = centrePosition.y;

	if (yPos >= centreYPos + bobDifference / 2 || yPos <= centreYPos - bobDifference / 2)
	{
		bobbingUp = !bobbingUp;
	}

}

void ExitPoint::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Frustum& frustum, Camera& cam)
{
	prop.Draw(pShader, vShader, frustum, cam);
}

bool ExitPoint::collidesWith(XMFLOAT3 otherPosition, float radius)
{	
	return BoundingSphere(prop.Pos(), size).Intersects(BoundingSphere(otherPosition, radius));
}