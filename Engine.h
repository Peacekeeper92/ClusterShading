#pragma once

using namespace Microsoft::WRL;
using namespace DirectX;
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

	void drawSponza();

	bool generateDevice();
	void generateQuad();

	static Engine* mInstance;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<IDXGISwapChain> mSwapChain;

	HWND mWindow;
	HINSTANCE mWindowInst;

	D3D11_VIEWPORT mViewport;

	ComPtr<ID3D11Texture2D> mSwapChainTex;
	ComPtr<ID3D11RenderTargetView> mSwapChainRTV;

	ComPtr<ID3D11Texture2D> mViewDepthTex;
	ComPtr<ID3D11DepthStencilView> mViewDepthDSV;
	ComPtr<ID3D11ShaderResourceView> mViewDepthSRV;

	static constexpr unsigned int GBUFFER_COUNT = 4;
	ComPtr<ID3D11Texture2D> mGBufferTex[GBUFFER_COUNT];
	ComPtr<ID3D11RenderTargetView> mGBufferRTV[GBUFFER_COUNT];
	ComPtr<ID3D11ShaderResourceView> mGBufferSRV[GBUFFER_COUNT];

	ComPtr<ID3D11Buffer> mQuadVB;
	ComPtr<ID3D11Buffer> mQuadIB;

	ComPtr<ID3D11VertexShader> mQuadVS;
	ComPtr<ID3D11PixelShader> mQuadPS;
	ComPtr<ID3D11InputLayout> mQuadIL;

	ComPtr<ID3D11RasterizerState> mDefaultRasterState;

	static constexpr unsigned int CLUSTER_STEPS = 4;
	static constexpr unsigned int CLUSTER_X = WIDTH / 10;
	static constexpr unsigned int CLUSTER_Y = HEIGHT / 10;
	static constexpr unsigned int CLUSTER_THREAD = 16;

	ComPtr<ID3D11Texture3D> mClusterMapTex;
	ComPtr<ID3D11UnorderedAccessView> mClusterMapUAV;
	ComPtr<ID3D11ComputeShader> mClusterCS;

	ComPtr<ID3D11Buffer> mSponzaVB;
	ComPtr<ID3D11Buffer> mSponzaIB;

	ComPtr<ID3D11VertexShader> mSponzaVS;
	ComPtr<ID3D11PixelShader> mSponzaPS;
	ComPtr<ID3D11InputLayout> mSponzaIL;
	ComPtr<ID3D11Buffer> mSponzaCB;

	ComPtr<ID3D11Buffer> mLightBuffer;
	ComPtr<ID3D11ShaderResourceView> mLightSRV;
	
	unsigned int mSponzaIndexCount;

	XMFLOAT4X4 mSponzaWorld;


	XMFLOAT4X4 mView;
	XMFLOAT4X4 mInvView;
	XMFLOAT4X4 mProjection;
	ComPtr<ID3D11Buffer> mCamera;
};

