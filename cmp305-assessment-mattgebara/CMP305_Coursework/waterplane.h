#pragma once
#include "PlaneMesh.h"
#include <vector>
using std::vector;

class WaterPlane :
	public PlaneMesh
{
public:
	WaterPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128, float ChangeScale = 1);
	~WaterPlane();

	void Resize(int newResolution);
	void Scale(float);
	void Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, bool smooth, float timer);
	void smooth();
	float size() { return terrainSize; }

	const inline int GetResolution() { return resolution; }
private:
	void CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices);
	void BuildHeightMap(float timer);


	float changeScale;
	const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
	float terrainSize = 100.0f;		//the width and height of our terrain
	vector<vector<float>> heightMap;
};

