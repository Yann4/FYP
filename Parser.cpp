#include "Parser.h"
using namespace DirectX;
using std::vector;

int Parser::readObjFile(ID3D11Device* _pd3dDevice, std::string fileName, MeshData* mesh, std::vector<Material>* materials)
{
	std::ifstream inf(fileName);

	if (!inf)
	{
		std::cout << "File could not be opened";
		return 1;
	}

	std::vector<XMFLOAT3> verticesVector;
	std::vector<XMFLOAT3> vertexNormals;
	std::vector<XMFLOAT3> uvCoords;

	std::vector<XMFLOAT3> vertexIndices;
	std::vector<XMFLOAT3> vnIndices;
	std::vector<XMFLOAT3> uvIndices;

	std::vector<MeshSection> parts;

	XMFLOAT3 bottomLeft = XMFLOAT3(NULL,NULL,NULL), topRight = XMFLOAT3(NULL,NULL,NULL);
	std::string mtlFileName;

	float vIndexOffset = 9999999.0f;
	float vnIndexOffset = 9999999.0f;
	float vtIndexOffset = 9999999.0f;

	int count = 0;
	int currentPart = 0;
	char cLine[100];

	while (!inf.eof())
	{
		ZeroMemory(&cLine, 100);

		inf.getline(cLine, 100);
		std::string line(cLine);
		
		if (line.find("mtllib") != std::string::npos)
		{
			unsigned int i = 7;
			while (line[i] != '.')
			{
				mtlFileName += line[i];
				i++;
			}

			mtlFileName.append(".txt");
		}

		if (line.find("usemtl ") != std::string::npos)
		{
			std::string mtlName;
			unsigned int i = 7;
			while (i < line.size() && line[i] != '\n')
			{
				mtlName += line[i];
				i++;
			}

			parts.push_back(MeshSection());

			if (currentPart != 0)
			{
				parts[currentPart - 1].endIndex = count - 1;
			}
			parts[currentPart].materialName = mtlName;
			parts[currentPart].startIndex = count;
			currentPart++;
		}

		if (line.find("g ") != std::string::npos)
		{
			MeshSection ms;
			ms.startIndex = count;

			if (currentPart != 0)
			{
				int index = currentPart - 1;
				parts[index].endIndex = count;
			}

			std::string name;
			int i = 2;
			while ((line[i] != '\n' && line[i] != '\0') && i < 30)
			{
				name += line[i];
				i++;
			}

			ms.sectionName = name;
			parts.push_back(ms);
		}

		if (line.find("v ") != std::string::npos)
		{
			float a, b, c;
			sscanf_s(line.c_str(), "v %f %f %f", &a, &b, &c);
			verticesVector.push_back(XMFLOAT3(a, c, b));
		}

		if (line.find("vn ") != std::string::npos)
		{
			float a, b, c;
			sscanf_s(line.c_str(), "vn %f %f %f", &a, &b, &c);
			vertexNormals.push_back(XMFLOAT3(a, b, c));
		}

		if (line.find("vt ") != std::string::npos)
		{
			float a, b, c = 0;
			sscanf_s(line.c_str(), "vt %f %f", &a, &b);
			b = 1.0f - b;
			uvCoords.push_back(XMFLOAT3(a, b, c));
		}

		if (line.find("f ") != std::string::npos)
		{
			float v1, v2, v3;
			float vn1, vn2, vn3;
			float vt1, vt2, vt3;
			sscanf_s(line.c_str(), "f %f/%f/%f %f/%f/%f %f/%f/%f", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			if (v1 < vIndexOffset)
			{
				vIndexOffset = v1;
			}
			if (v2 < vIndexOffset)
			{
				vIndexOffset = v2;
			}
			if (v3 < vIndexOffset)
			{
				vIndexOffset = v3;
			}

			if (vn1 < vnIndexOffset)
			{
				vnIndexOffset = vn1;
			}
			if (vn2 < vnIndexOffset)
			{
				vnIndexOffset = vn2;
			}
			if (vn3 < vnIndexOffset)
			{
				vnIndexOffset = vn3;
			}

			if (vt1 < vtIndexOffset)
			{
				vtIndexOffset = vt1;
			}
			if (vt2 < vtIndexOffset)
			{
				vtIndexOffset = vt2;
			}
			if (vt3 < vtIndexOffset)
			{
				vtIndexOffset = vt3;
			}

			vertexIndices.push_back(XMFLOAT3(v1, v2, v3));
			vnIndices.push_back(XMFLOAT3(vn1, vn2, vn3));
			uvIndices.push_back(XMFLOAT3(vt1, vt2, vt3));

			count += 3;
		}
	}

	if (currentPart > 0)
	{
		currentPart--;
	}
	parts.at(currentPart).endIndex = count;

	inf.close();
	vector<WORD> indices = vector<WORD>(vertexIndices.size() * 3);
	vector<SimpleVertex> vertices = vector<SimpleVertex>(vertexIndices.size() * 3);

	count = 0;

	for (WORD i = 0; i < vertexIndices.size(); i++)
	{
		//X index
		indices[count] = (WORD)count;
		
		SimpleVertex temp;
		temp.Normal = vertexNormals[(unsigned int)(vnIndices[i].x - vnIndexOffset)];
		temp.Pos = verticesVector[(unsigned int)(vertexIndices[i].x - vIndexOffset)];

		//NOT READING IN Z
		temp.TexC.x = uvCoords[(unsigned int)(uvIndices[i].x - vtIndexOffset)].x;
		temp.TexC.y = uvCoords[(unsigned int)(uvIndices[i].x - vtIndexOffset)].y;

		vertices[count] = temp;
		count++;

		//Y index
		indices[count] = (WORD)count;

		temp.Normal = vertexNormals[(unsigned int)(vnIndices[i].y - vnIndexOffset)];
		temp.Pos = verticesVector[(unsigned int)(vertexIndices[i].y - vIndexOffset)];

		//NOT READING IN Z
		temp.TexC.x = uvCoords[(unsigned int)(uvIndices[i].y - vtIndexOffset)].x;
		temp.TexC.y = uvCoords[(unsigned int)(uvIndices[i].y - vtIndexOffset)].y;

		vertices[count] = temp;
		count++;

		//Z index
		indices[count] = (WORD)count;

		temp.Normal = vertexNormals[(unsigned int)(vnIndices[i].z - vnIndexOffset)];
		temp.Pos = verticesVector[(unsigned int)(vertexIndices[i].z - vIndexOffset)];

		//NOT READING IN Z
		temp.TexC.x = uvCoords[(unsigned int)(uvIndices[i].z - vtIndexOffset)].x;
		temp.TexC.y = uvCoords[(unsigned int)(uvIndices[i].z - vtIndexOffset)].y;

		vertices[count] = temp;
		count++;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * count;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &(vertices)[0];

	_pd3dDevice->CreateBuffer(&bd, &InitData, &(mesh->vertexBuffer));

	bd.ByteWidth = sizeof(WORD) * count;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &(indices)[0];
	_pd3dDevice->CreateBuffer(&bd, &InitData, &(mesh->indexBuffer));

	mesh->numIndices = count;
	mesh->parts = parts;

	if (materials != nullptr)
	{
		readMtlFile(mtlFileName, materials);

		for (unsigned int i = 0; i < mesh->parts.size(); i++)
		{
			std::string mtlName = mesh->parts[i].materialName;
			
			for (unsigned int j = 0; j < materials->size(); j++)
			{
				std::string comp = materials->at(j).name;

				if (comp.find(mtlName) != std::string::npos)
				{
					mesh->parts.at(i).material = &(materials->at(j));
					break;
				}
			}
		}
	}
	
	for (unsigned int i = 0; i < mesh->parts.size(); i++)
	{
		float maxX = NULL, maxY = NULL, maxZ = NULL;
		float minX = NULL, minY = NULL, minZ = NULL;

		for (int j = mesh->parts.at(i).startIndex; j < mesh->parts.at(i).endIndex; j++)
		{
			if (maxX == NULL)
			{
				maxX = vertices[j].Pos.x;
				maxY = vertices[j].Pos.y;
				maxZ = vertices[j].Pos.z;

				minX = vertices[j].Pos.x;
				minY = vertices[j].Pos.y;
				minZ = vertices[j].Pos.z;
			}

			if (vertices[j].Pos.x > maxX)
			{
				maxX = vertices[j].Pos.x;
			}
			if (vertices[j].Pos.y > maxY)
			{
				maxY = vertices[j].Pos.y;
			}
			if (vertices[j].Pos.z > maxZ)
			{
				maxZ = vertices[j].Pos.z;
			}

			if (vertices[j].Pos.x < minX)
			{
				minX = vertices[j].Pos.x;
			}
			if (vertices[j].Pos.y < minY)
			{
				minY = vertices[j].Pos.y;
			}
			if (vertices[j].Pos.z < minZ)
			{
				minZ = vertices[j].Pos.z;
			}
		}

		mesh->parts.at(i).centre = XMFLOAT3(minX + ((maxX - minX) / 2.0f), minY + ((maxY - minY) / 2.0f), minZ + ((maxZ - minZ) / 2.0f));
		mesh->parts.at(i).size = XMFLOAT3(maxX - minX, maxY - minY, maxZ - minZ);

		if (bottomLeft.x == NULL)
		{
			bottomLeft = XMFLOAT3(minX, minY, minZ);
			topRight = XMFLOAT3(maxX, maxY, maxZ);
		}
		else
		{
			if (bottomLeft.x > minX)
			{
				bottomLeft.x = minX;
			}
			if (bottomLeft.y > minY)
			{
				bottomLeft.y = minY;
			}
			if (bottomLeft.z > minZ)
			{
				bottomLeft.z = minZ;
			}

			if (topRight.x < maxX)
			{
				topRight.x = maxX;
			}
			if (topRight.y < maxY)
			{
				topRight.y = maxY;
			}
			if (topRight.z < maxZ)
			{
				topRight.z = maxZ;
			}
		}
	}

	mesh->size = XMFLOAT3(topRight.x - bottomLeft.x, topRight.y - bottomLeft.y, topRight.z - bottomLeft.z);
	return 0;
}

int Parser::readMtlFile(std::string fileName, std::vector<Material>* materials)
{
	std::ifstream inf(fileName);
	int mtlcount = -1;

	if (!inf)
	{
		std::cout << "File could not be opened";
		return 1;
	}


	while (!inf.eof())
	{
		char line[128];
		inf.getline(line, 128);

		if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
		{
			mtlcount++;

			std::string mtlName;
			Material mtl;
			mtl.ambient = XMFLOAT4(0, 0, 0, 0);
			mtl.diffuse = XMFLOAT4(0, 0, 0, 0);
			mtl.specular = XMFLOAT4(0, 0, 0, 0);

			materials->push_back(mtl);

			int i = 7;
			while (line[i] != '\n' && i < 40)
			{
				mtlName += line[i];
				i++;
			}
			materials->at(mtlcount).name = mtlName;
		}

		if (line[0] == 'K' && line[1] == 'a')
		{
			float r, g, b;

			sscanf_s(line, "\tKa %f %f %f", &r, &g, &b);

			materials->at(mtlcount).ambient = XMFLOAT4(r, g, b, 1.0);
		}

		if (line[0] == 'K' && line[1] == 'd')
		{
			float r, g, b;

			sscanf_s(line, "\tKd %f %f %f", &r, &g, &b);

			materials->at(mtlcount).diffuse = XMFLOAT4(r, g, b, 1.0);
		}

		if (line[0] == 'K' && line[1] == 's')
		{
			float r, g, b;

			sscanf_s(line, "\tKs %f %f %f", &r, &g, &b);

			materials->at(mtlcount).specular = XMFLOAT4(r, g, b, 1.0);
		}
	}

	inf.close();
	return 0;
}