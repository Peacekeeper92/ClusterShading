#include "pch.h"
#include "Engine.h"
#include"FBXLoader.h"
//using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

Engine* Engine::mInstance = nullptr;

void Engine::Init()
{
	HRESULT result;
	result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(mSwapChainTex.GetAddressOf()));
	assert(result == S_OK);

	D3D11_RASTERIZER_DESC rasterizer = CD3D11_RASTERIZER_DESC(D3D11_FILL_SOLID, D3D11_CULL_NONE, true, 0, 0.0f, 0.0, true, false, false, false);
	result = mDevice->CreateRasterizerState(&rasterizer, mDefaultRasterState.GetAddressOf());
	assert(result == S_OK);

	D3D11_TEXTURE2D_DESC swapChainTexDesc;
	D3D11_RENDER_TARGET_VIEW_DESC swapChainRTVDesc{};
	mSwapChainTex->GetDesc(&swapChainTexDesc);
	swapChainRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	swapChainRTVDesc.Format = swapChainTexDesc.Format;

	result = mDevice->CreateRenderTargetView(mSwapChainTex.Get(), &swapChainRTVDesc, mSwapChainRTV.GetAddressOf());
	assert(result == S_OK);

	D3D11_TEXTURE2D_DESC viewDepthTex{};
	viewDepthTex.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	viewDepthTex.Format = DXGI_FORMAT_R24G8_TYPELESS;
	viewDepthTex.Width = WIDTH;
	viewDepthTex.Height = HEIGHT;
	viewDepthTex.ArraySize = 1;
	viewDepthTex.SampleDesc.Count = 1;

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDepthDSV{};
	viewDepthDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	viewDepthDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDepthSRV{};
	viewDepthSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	viewDepthSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDepthSRV.Texture2D.MipLevels = 1;

	result = mDevice->CreateTexture2D(&viewDepthTex, nullptr, mViewDepthTex.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateDepthStencilView(mViewDepthTex.Get(), &viewDepthDSV, mViewDepthDSV.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateShaderResourceView(mViewDepthTex.Get(), &viewDepthSRV, mViewDepthSRV.GetAddressOf());
	assert(result == S_OK);

	D3D11_TEXTURE2D_DESC gbufferDescs[GBUFFER_COUNT] = {
		CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R32G32B32A32_FLOAT, WIDTH, HEIGHT, 1U, 0U, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R32G32B32A32_FLOAT, WIDTH, HEIGHT, 1U, 0U, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R32G32_FLOAT, WIDTH, HEIGHT, 1U, 0U, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
	};

	D3D11_RENDER_TARGET_VIEW_DESC gbufferRTVDescs[GBUFFER_COUNT] =
	{
		CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32B32A32_FLOAT),
		CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32B32A32_FLOAT),
		CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32_FLOAT),
	};

	string gbufferNames[GBUFFER_COUNT] =
	{
		"Position",
		"Normal",
		"Texcoord"
	};

	for (unsigned int i = 0; i < GBUFFER_COUNT; i++)
	{
		result = mDevice->CreateTexture2D(&gbufferDescs[i], nullptr, mGBufferTex[i].GetAddressOf());
		assert(result == S_OK);
		
		result = mDevice->CreateRenderTargetView(mGBufferTex[i].Get(), &gbufferRTVDescs[i], mGBufferRTV[i].GetAddressOf());
		assert(result == S_OK);
	}

	D3D11_TEXTURE3D_DESC clusterMapDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC clusterMapUAVDesc{};
	clusterMapDesc.Format = DXGI_FORMAT_R32_UINT;
	clusterMapDesc.MipLevels = 1;
	clusterMapDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	clusterMapDesc.Width = CLUSTER_X;
	clusterMapDesc.Height = CLUSTER_Y;
	clusterMapDesc.Depth = CLUSTER_STEPS;

	clusterMapUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	clusterMapUAVDesc.Texture3D.WSize = CLUSTER_STEPS;

	result = mDevice->CreateTexture3D(&clusterMapDesc, nullptr, mClusterMapTex.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateUnorderedAccessView(mClusterMapTex.Get(), &clusterMapUAVDesc, mClusterMapUAV.GetAddressOf());
	assert(result == S_OK);

	FBXLoader sponza;

	sponza.Load("Cube/Cube.fbx");

	auto vertices = sponza.vertices;
	auto indices = sponza.indices;

	mSponzaIndexCount = indices.size();

	D3D11_BUFFER_DESC vertexBufferDesc{};
	D3D11_BUFFER_DESC indexBufferDesc{};
	D3D11_SUBRESOURCE_DATA vertexSubData, indexSubData{};

	vertexBufferDesc.ByteWidth = vertices.size() * sizeof(Vertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexSubData.pSysMem = vertices.data();

	indexBufferDesc.ByteWidth = indices.size() * 4; // sizeof unsigned int
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexSubData.pSysMem = indices.data();

	result = mDevice->CreateBuffer(&vertexBufferDesc, &vertexSubData, mSponzaVB.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateBuffer(&indexBufferDesc, &indexSubData, mSponzaIB.GetAddressOf());
	assert(result == S_OK);

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = WIDTH;
	mViewport.Height = HEIGHT;
	mViewport.MaxDepth = 1.0f;
	mViewport.MinDepth = 0.0f;

	D3D11_BUFFER_DESC constantBuffer{};
	constantBuffer.ByteWidth = sizeof(XMFLOAT4X4) * 3;
	constantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


	result = mDevice->CreateBuffer(&constantBuffer, nullptr, mSponzaCB.GetAddressOf());
	assert(result == S_OK);

	XMStoreFloat4x4(&mSponzaWorld, XMMatrixIdentity());
	//mSponzaWorld._24 = -1.0f;
	
	XMStoreFloat4x4(&mView, XMMatrixLookAtLH(
		XMVectorSet(0, 0, 0, 1),
		XMVectorSet(1, 0, 0, 1),
		XMVectorSet(0, 1, 0, 1)
	));



	XMStoreFloat4x4(&mProjection,
					XMMatrixPerspectiveFovLH(
						XMConvertToRadians(60.0f),
						1.333, 0.01f, 1000.0f));

	XMFLOAT4X4 matrices[] = { mSponzaWorld, mView, mProjection };

	mContext->UpdateSubresource(mSponzaCB.Get(), 0, nullptr, matrices, 0, 0);

	ID3DBlob* vBlob,*pBlob, *errBlob;

	result = D3DCompileFromFile(L"Sponza.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, D3DCOMPILE_DEBUG, &vBlob, &errBlob);
	assert(result == S_OK);
	result = D3DCompileFromFile(L"Sponza.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, D3DCOMPILE_DEBUG, &pBlob, &errBlob);
	assert(result == S_OK);
	result = mDevice->CreateVertexShader(vBlob->GetBufferPointer(), vBlob->GetBufferSize(), nullptr, mSponzaVS.GetAddressOf());
	assert(result == S_OK);
	
	result = mDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, mSponzaPS.GetAddressOf());
	assert(result == S_OK);

	D3D11_INPUT_ELEMENT_DESC vertexElements[] =
	{
		/*    LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	} 	D3D11_INPUT_ELEMENT_DESC;*/
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	result = mDevice->CreateInputLayout(vertexElements, ARRAYSIZE(vertexElements), vBlob->GetBufferPointer(), vBlob->GetBufferSize(), mSponzaIL.GetAddressOf());
	assert(result == S_OK);

	mContext->OMSetRenderTargets(1, mSwapChainRTV.GetAddressOf(), mViewDepthDSV.Get());
}

void Engine::Update(float delta)
{
	mContext->ClearRenderTargetView(mSwapChainRTV.Get(), Colors::Green);
	mContext->ClearDepthStencilView(mViewDepthDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	drawSponza();
}

void Engine::Render(float delta)
{
	mSwapChain->Present(0, 0);
}

void Engine::Release()
{
	mSwapChainTex->Release();
	mSwapChainRTV->Release();

	
}

Engine::Engine(HWND window, HINSTANCE inst)
	: mWindow(window), mWindowInst(inst)
{
	generateDevice();
}

void Engine::drawSponza()
{
	static unsigned int Strides[] = { sizeof(Vertex) };
	static unsigned int Offsets[] = { 0 };
	
	mContext->VSSetShader(mSponzaVS.Get(), nullptr, 0);
	mContext->PSSetShader(mSponzaPS.Get(), nullptr, 0);

	mContext->VSSetConstantBuffers(0, 1, mSponzaCB.GetAddressOf());
	mContext->RSSetViewports(1, &mViewport);
	
	mContext->IASetIndexBuffer(mSponzaIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	mContext->IASetVertexBuffers(0, 1, mSponzaVB.GetAddressOf(), Strides, Offsets);
	mContext->IASetInputLayout(mSponzaIL.Get());
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	
	mContext->DrawIndexed(mSponzaIndexCount, 0, 0);
	
}

bool Engine::generateDevice()
{
	DWORD flag = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	D3D_FEATURE_LEVEL featureLevel[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = WIDTH;
	swapChainDesc.BufferDesc.Height = HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 144;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = mWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = true;

#ifdef _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	auto result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag,
												nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
												mSwapChain.GetAddressOf(), mDevice.GetAddressOf(),
												featureLevel, mContext.GetAddressOf());

	assert(result == S_OK);
	return true;
}
