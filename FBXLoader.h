#pragma once
#include"Vertex.h"

using namespace DirectX;
using namespace std;
class FBXLoader
{
private:
	FbxScene* Scene;
	FbxManager* mManager;

	vector<XMFLOAT4> Positions;
	vector<XMFLOAT3> Normals;
	vector<XMFLOAT2> UVs;

public:
	bool Load(string Path);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void LoadNode(FbxNode* Node);
	void GetVertex(FbxMesh* Mesh);
	XMFLOAT3 ReadNormal(FbxMesh* Mesh, int CPI, int VertexCount);
};