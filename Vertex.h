#pragma once
struct Vertex
{
	DirectX::XMFLOAT4 mPosition;
	DirectX::XMFLOAT3 mNormal;
	DirectX::XMFLOAT2 mTexcoord;
};

struct QuadVertex
{
	DirectX::XMFLOAT4 mPosition;
	DirectX::XMFLOAT2 mTexcoord;
};