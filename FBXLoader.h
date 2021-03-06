#pragma once
#include"Vertex.h"

using namespace DirectX;
using namespace std;
class FBXLoader
{
private:
	FbxScene* Scene;

	XMFLOAT4* Positions = nullptr;
	XMFLOAT3* Normals = nullptr;
	XMFLOAT2* UVs = nullptr;
public:
	bool Load(string Path);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void LoadNode(FbxNode* Node);
	void GetVertex(FbxMesh* Mesh);
	XMFLOAT3 ReadNormal(FbxMesh* Mesh, int CPI, int VertexCount);
};