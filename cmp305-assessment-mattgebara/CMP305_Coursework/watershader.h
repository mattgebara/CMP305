#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	struct  WaterBufferType
	{
		float terrainSize;
		float resolution;
		float offSet;
		float padding;
	};


public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, XMFLOAT3 values);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11BlendState* pBlend;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* waterBuffer;
};

