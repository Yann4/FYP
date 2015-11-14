#pragma once
#include <istream>
#include <iostream>
#include <string>
#include <fstream>
#include <d3d11.h>
#include <vector>

#include "data structs.h"
#include "GameObject.h"

class Parser
{
public:
	Parser(){};
	int readMtlFile(std::string fileName, std::vector<Material>* materials);
	int readObjFile(ID3D11Device* _pd3dDevice, std::string fileName, MeshData* mesh, std::vector<Material>* materials);
};