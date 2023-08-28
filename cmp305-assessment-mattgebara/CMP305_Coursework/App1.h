// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainMesh1.h"
#include "WaterPlane.h"
#include "WaterShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	LightShader* shader;
	WaterShader* waterShader;
	TerrainMesh1* m_Terrain;
	WaterPlane* waterTerrain;

	Light* light;

	float ChangeY = 1, water; 
	float ChangeScale = 1;


	int terrainResolution = 128;
	float newamount1 = 1.f;
	float newamount2 = 1.f;
	float newamount3 = 1.f;
	float newiterations = 1;


};

#endif