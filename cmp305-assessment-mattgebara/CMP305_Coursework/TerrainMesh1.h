#pragma once
#include "PlaneMesh.h"
#include "PerlinNoise.h"
#include <vector>
#include <cmath>

using std::vector;
struct Vector3f
{
	float x;
	float y;
	float z;
};

struct Line
{
	Vector3f start;
	Vector3f end;
};
class TerrainMesh1 :
	public PlaneMesh {
public:
	TerrainMesh1(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128, float ChangeY = -1, float ChangeScale = 1);
	~TerrainMesh1();


	void Resize(int newResolution, float);
	void Scale(float);
	void Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, bool smoothy);
	void smooth();
	void fault();

	float length(Line);
	const inline int GetResolution() { return resolution; }


	const inline float Checkamount1() { return amount1; }

	const inline float Checkamount2() { return amount2; }


	const inline int getIterations() { return iterations; }



	void BuildHeightMap(float newamount1, float newamount2, float newamount3);
	void perlinNoise(float newamount1, float newamount2);

	void setiterations(int new_iterations) { iterations = new_iterations; };
	void setheightmap(bool new_heightmap) { heightchangeon = new_heightmap; };


private:
	void CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices);

	bool heightchangeon = false;

	float changeY;
	float changeScale;
	const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
	float terrainSize = 100.0f;		//the width and height of our terrain
	vector<vector<float>> heightMap;

	float* heightmap;

	float amount1;
	float amount2;
	float amount3;


	float iterations = 0;

};
