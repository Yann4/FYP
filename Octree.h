#pragma once
#include <array>
#include <vector>

#include <DirectXMath.h>

struct AABB
{
	DirectX::XMFLOAT3 centre;
	DirectX::XMFLOAT3 hSize;

	AABB() :centre(DirectX::XMFLOAT3(0, 0, 0)), hSize(DirectX::XMFLOAT3(0, 0, 0))
	{}

	AABB(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 hSize) :centre(centre), hSize(hSize)
	{}

private:
	bool testAxis(DirectX::XMFLOAT3 axis, float minA, float maxA, float minB, float maxB)
	{
		DirectX::XMVECTOR axisV = XMLoadFloat3(&axis);
		float axisLSq = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axisV, axisV));

		if (axisLSq < 1.0e-8f)
		{
			return true;
		}

		float d0 = maxB - minA; //Left side
		float d1 = maxA - minB; //Right side

		if (d0 <= 0.0f || d1 <= 0.0f)
		{
			return false;
		}

		return true;
	}

public:
	bool intersects(AABB other)
	{
		DirectX::XMFLOAT3 axis = DirectX::XMFLOAT3(1, 0, 0); //Check x axis

		if (!testAxis(axis, centre.x - hSize.x, centre.x + hSize.x, other.centre.x - other.hSize.x, other.centre.x + other.hSize.x))
		{
			return false;
		}

		axis = DirectX::XMFLOAT3(0, 1, 0); //Check y axis
		if (!testAxis(axis, centre.y - hSize.y, centre.y + hSize.y, other.centre.y - other.hSize.y, other.centre.y + other.hSize.y))
		{
			return false;
		}

		axis = DirectX::XMFLOAT3(0, 0, 1); //Check z axis
		if (!testAxis(axis, centre.z - hSize.z, centre.z + hSize.z, other.centre.z - other.hSize.z, other.centre.z + other.hSize.z))
		{
			return false;
		}
		return true;
	}
};



template <typename T>
class Octree
{
	struct Element
	{
		AABB bounds;
		T data;

		Element(T data, AABB box) : data(data), bounds(box) {}
	};
private:
	std::array<Octree*, 8> children;

	AABB boundingBox;

	std::vector<Element> elements;

	unsigned int maxElements = 5;
	DirectX::XMFLOAT3 minSize = DirectX::XMFLOAT3(1, 1, 1);

private:
	bool subdivide();
public:
	Octree();
	Octree(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 halfSize);

	~Octree();

	void insert(T toInsert, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 size);

	inline AABB bounds() { return boundingBox; };

	std::vector<T*> getElementsInBounds(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 halfSize);
	std::vector<T*> getAllElements();
};

template <typename T>
Octree<T>::Octree()
{
	children = std::array<Octree*, 8>();
	children.fill(nullptr);

	boundingBox = AABB();

	elements = std::vector<Element>();
}

template <typename T>
Octree<T>::Octree(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 halfSize)
{
	boundingBox = AABB(centre, halfSize);
	children = std::array<Octree*, 8>();
	children.fill(nullptr);

	elements = std::vector<Element>();
}

template <typename T>
Octree<T>::~Octree()
{
	if (children[0] != nullptr)
	{
		for (auto child : children)
		{
			delete child;
		}
	}
}

template <typename T>
bool Octree<T>::subdivide()
{
	if (children[0] != nullptr)
	{
		return false;
	}
	DirectX::XMFLOAT3 centre = boundingBox.centre;
	DirectX::XMFLOAT3 qSize = DirectX::XMFLOAT3(boundingBox.hSize.x / 2, boundingBox.hSize.y / 2, boundingBox.hSize.z / 2);

	if (qSize.x < minSize.x || qSize.y < minSize.y || qSize.z < minSize.z)
	{
		return false;
	}

	DirectX::XMFLOAT3 newCentre = DirectX::XMFLOAT3(centre.x + qSize.x, centre.y + qSize.y, centre.z + qSize.z);
	children[0] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x - qSize.x, centre.y + qSize.y, centre.z + qSize.z);
	children[1] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x - qSize.x, centre.y - qSize.y, centre.z + qSize.z);
	children[2] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x + qSize.x, centre.y + qSize.y, centre.z + qSize.z);
	children[3] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x + qSize.x, centre.y + qSize.y, centre.z - qSize.z);
	children[4] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x - qSize.x, centre.y + qSize.y, centre.z - qSize.z);
	children[5] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x - qSize.x, centre.y - qSize.y, centre.z - qSize.z);
	children[6] = new Octree(newCentre, qSize);

	newCentre = DirectX::XMFLOAT3(centre.x + qSize.x, centre.y + qSize.y, centre.z - qSize.z);
	children[7] = new Octree(newCentre, qSize);

	for (auto object : elements)
	{
		for (auto child : children)
		{
			if (child->bounds().intersects(object.bounds))
			{
				child->insert(object.data, object.bounds.centre, object.bounds.hSize);
				break;
			}
		}
	}
	elements.clear();

	return true;
}

template <typename T>
void Octree<T>::insert(T toInsert, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 halfSize)
{
	AABB objBox = AABB(pos, halfSize);
	bool inserted = false;

	//If has children, insert in child
	if (children[0] != nullptr)
	{
		for (auto child : children)
		{
			if (child->bounds().intersects(objBox))
			{
				child->insert(toInsert, pos, halfSize);
				inserted = true;
				break;
			}
		}
	}
	else
	{
		if (elements.size() < maxElements)
		{
			elements.push_back(Element(toInsert, objBox));
			inserted = true;
		}
		else
		{
			if (subdivide())
			{
				for (auto child : children)
				{
					if (child->bounds().intersects(objBox))
					{
						child->insert(toInsert, pos, halfSize);
						inserted = true;
						break;
					}
				}
				if (!inserted)
				{
					elements.push_back(Element(toInsert, objBox));
					inserted = true;
				}
			}
			else
			{
				elements.push_back(Element(toInsert, objBox));
				inserted = true;
			}
		}
	}
	if (!inserted)
	{
		elements.push_back(Element(toInsert, objBox));
	}
}

template <typename T>
std::vector<T*> Octree<T>::getElementsInBounds(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 halfSize)
{
	std::vector<T*> toRet = std::vector<T*>();
	AABB examine = AABB(centre, halfSize);

	if (boundingBox.intersects(examine))
	{
		for (int i = 0; i < elements.size(); i++)
		{
			toRet.push_back(&(elements.at(i).data));
		}

		if (children[0] != nullptr)
		{
			for (auto child : children)
			{
				std::vector<T*> tmp = child->getElementsInBounds(centre, halfSize);
				toRet.insert(toRet.end(), tmp.begin(), tmp.end());
			}
		}
	}

	return toRet;
}

template <typename T>
std::vector<T*> Octree<T>::getAllElements()
{
	std::vector<T*> toRet = std::vector<T*>();

	for (int i = 0; i < elements.size(); i++)
	{
		toRet.push_back(&(elements.at(i).data));
	}

	if (children[0] != nullptr)
	{
		for (auto child : children)
		{
			std::vector<T*> tmp = child->getAllElements();
			toRet.insert(toRet.end(), tmp.begin(), tmp.end());
		}
	}

	return toRet;
}