#pragma once
#include<DirectXMath.h>
using namespace DirectX;

//// 32 byte
//struct DirectionalLight
//{
//	XMFLOAT4 mDirection;
//	XMFLOAT3 mColor;
//	float mIntensity;
//
//	DirectionalLight(XMVECTOR direction, XMVECTORF32 color, float intensity)
//		: mIntensity(intensity)
//	{
//		XMStoreFloat4(&mDirection, direction);
//		XMStoreFloat3(&mColor, color);
//	}
//};

struct PointLight
{
	XMFLOAT4 mPosition;
	XMFLOAT3 mColor;
	float mIntensity;
	float mRadius;
	PointLight(XMVECTOR position, XMVECTORF32 color, float intensity, float radius)
		: mIntensity(intensity), mRadius(radius)
	{
		XMStoreFloat4(&mPosition, position);
		XMStoreFloat3(&mColor, color);
	}
};