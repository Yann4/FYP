#include "Spline.h"
using namespace DirectX;

Spline::Spline()
{
	context = nullptr;
	controlPoints = std::vector<XMFLOAT3>();
	linePoints = std::vector<XMFLOAT3>(100);
}

Spline::Spline(std::vector<XMFLOAT3> controlPoints, ID3D11DeviceContext * context): controlPoints(controlPoints), context(context)
{
	linePoints = std::vector<XMFLOAT3>(100);
}

void Spline::generateLine()
{
	unsigned int perc = 0;
}
