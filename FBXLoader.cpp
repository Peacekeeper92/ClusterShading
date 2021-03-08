#include"pch.h"
#include "FBXLoader.h"

using namespace std;

bool FBXLoader::Load(string Path)
{
	FbxManager* Manager = FbxManager::Create();
	mManager = Manager;
	FbxIOSettings* IOSetting = FbxIOSettings::Create(Manager, IOSROOT);
	Manager->SetIOSettings(IOSetting);
	FbxImporter* Importer = FbxImporter::Create(Manager, "");

	bool result = Importer->Initialize(Path.c_str(), -1, Manager->GetIOSettings());
	assert(result == true);

	Vertex vt;
	
	Scene = FbxScene::Create(Manager, "Scene");

	Importer->Import(Scene);
	FbxGeometryConverter geoConvert(Manager);
	bool triangulateResult = geoConvert.Triangulate(Scene, true);



	Importer->Destroy();

	FbxNode* Root = Scene->GetRootNode();
	
	LoadNode(Root);

	IOSetting->Destroy();
	Manager->Destroy();

	return true;
}

void FBXLoader::LoadNode(FbxNode* Node)
{
	FbxNodeAttribute* NodeAtt = Node->GetNodeAttribute();
	static int MeshCounter = 0;
	Vertex vt;

	XMFLOAT4 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;

	if (NodeAtt)
	{
		if (NodeAtt->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* Mesh = Node->GetMesh();
			FbxGeometryConverter conv(mManager);

				//conv.Triangulate(Mesh, true);
				GetVertex(Mesh);
				bool b = Mesh->IsTriangleMesh();
				unsigned int polyVertCount = Mesh->GetPolygonVertexCount();
				unsigned int TriCount = Mesh->GetPolygonCount();
				unsigned int VertexCount = 0;

				for (unsigned int i = 0; i < TriCount; i++)
				{
					int index = Mesh->GetPolygonVertexIndex(i);
					for (unsigned int j = 0; j < 3; j++)
					{
						int CPI = Mesh->GetPolygonVertex(i, j);
						XMFLOAT4& Position = Positions[CPI];
						//XMFLOAT3 Normal = ReadNormal(Mesh, CPI, VertexCount);

						vt.mPosition = Position;
						//vt.mNormal = Normal;

						vertices.push_back(vt);
						indices.push_back(index + j);

						VertexCount++;
					}
				}

			}
		
		MeshCounter++;

	}


	const int ChildCount = Node->GetChildCount();
	for (int i = 0; i < ChildCount; i++)
	{
		LoadNode(Node->GetChild(i));
	}



}

void FBXLoader::GetVertex(FbxMesh* Mesh)
{
	unsigned int Count = Mesh->GetControlPointsCount();
	Positions.resize(Count);
	Normals.resize(Count);
	UVs.resize(Count);

	for (int i = 0; i < Count; i++)
	{
		XMFLOAT4 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 UV;
		
		Position.x = static_cast<float>(Mesh->GetControlPointAt(i).mData[0]);
		Position.y = static_cast<float>(Mesh->GetControlPointAt(i).mData[1]);
		Position.z = static_cast<float>(Mesh->GetControlPointAt(i).mData[2]);

		Positions[i] = Position;

	}

}

XMFLOAT3 FBXLoader::ReadNormal(FbxMesh* Mesh, int CPI, int VertexCount)
{
	if (Mesh->GetElementNormalCount() < 1)
	{
		assert(true);
	}

	FbxGeometryElementNormal* VertexNormal = Mesh->GetElementNormal();
	XMFLOAT3 Normal = XMFLOAT3(0,0,0);

	switch (VertexNormal->GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (VertexNormal->GetReferenceMode())
			{
				case FbxGeometryElement::eNormal:
				{
					Normal.x = static_cast<float>(VertexNormal->GetDirectArray().GetAt(CPI).mData[0]);
					Normal.y = static_cast<float>(VertexNormal->GetDirectArray().GetAt(CPI).mData[1]);
					Normal.z = static_cast<float>(VertexNormal->GetDirectArray().GetAt(CPI).mData[2]);
				} break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int Index = VertexNormal->GetIndexArray().GetAt(CPI);
					Normal.x = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[0]);
					Normal.y = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[1]);
					Normal.z = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[2]);
				} break;





			}
			break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (VertexNormal->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					Normal.x = static_cast<float>(VertexNormal->GetDirectArray().GetAt(VertexCount).mData[0]);
					Normal.y = static_cast<float>(VertexNormal->GetDirectArray().GetAt(VertexCount).mData[1]);
					Normal.z = static_cast<float>(VertexNormal->GetDirectArray().GetAt(VertexCount).mData[2]);
				} break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int Index = VertexNormal->GetIndexArray().GetAt(VertexCount);
					Normal.x = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[0]);
					Normal.y = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[1]);
					Normal.z = static_cast<float>(VertexNormal->GetDirectArray().GetAt(Index).mData[2]);
				} break;
			}
		}
		break;
		}

	}


	return Normal;
}