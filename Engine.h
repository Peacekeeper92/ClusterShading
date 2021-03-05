#pragma once

using namespace Microsoft::WRL;

class Engine
{
public:
	void Init();
	void Update(float delta);
	void Render(float delta);
	void Release();
	
	Engine(HWND window, HINSTANCE inst);

	static Engine* GetInstance(HWND window = nullptr, HINSTANCE windowInst = nullptr)
	{
		if (mInstance == nullptr)
		{
			mInstance = new Engine(window, windowInst);
		}

		return mInstance;
	}
	
private:
	bool generateDevice();

	static Engine* mInstance;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<IDXGISwapChain> mSwapChain;

	HWND mWindow;
	HINSTANCE mWindowInst;

	ComPtr<ID3D11Texture2D> mSwapChainTex;
	ComPtr<ID3D11RenderTargetView> mSwapChainRTV;

	ComPtr<ID3D11Texture2D> mViewDepthTex;
	ComPtr<ID3D11DepthStencilView> mViewDepthDSV;
	ComPtr<ID3D11ShaderResourceView> mViewDepthSRV;

	static constexpr unsigned int GBUFFER_COUNT = 3;
	ComPtr<ID3D11Texture2D> mGBufferTex[GBUFFER_COUNT];
	ComPtr<ID3D11RenderTargetView> mGBufferRTV[GBUFFER_COUNT];

	static constexpr unsigned int CLUSTER_STEPS = 8;
	static constexpr unsigned int CLUSTER_X = 80;
	static constexpr unsigned int CLUSTER_Y = 60;
	static constexpr unsigned int CLUSTER_THREAD = 10;

	ComPtr<ID3D11Texture3D> mClusterMapTex;
	ComPtr<ID3D11UnorderedAccessView> mClusterMapUAV;

	ComPtr<ID3D11Buffer> mSponzaVB;
	ComPtr<ID3D11Buffer> mSponzaIB;


};

