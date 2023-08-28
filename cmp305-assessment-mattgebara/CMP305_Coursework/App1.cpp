#include "App1.h"

App1::App1()
{
	m_Terrain = nullptr;
	shader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load textures
	textureMgr->loadTexture(L"grass", L"res/othergrass.png");
	textureMgr->loadTexture(L"water", L"res/water.png");
	textureMgr->loadTexture(L"Rock", L"res/rocks.png");
	textureMgr->loadTexture(L"Snow", L"res/snow.png");
	textureMgr->loadTexture(L"Mud", L"res/mud.png");


	// Create Mesh object and shader object
	m_Terrain = new TerrainMesh1(renderer->getDevice(), renderer->getDeviceContext());
	waterTerrain = new WaterPlane(renderer->getDevice(), renderer->getDeviceContext());

	shader = new LightShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	water = 0;

	//set camera position
	camera->setPosition(50.f, 50.f, -75.f);
	camera->setRotation(25.f, 0.f, 0.f);

	// Initialise light
	light = new Light();
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(0.5f, -0.6f, 0.5f);
	srand(time(nullptr));

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (waterTerrain)
	{
		delete waterTerrain;
		waterTerrain = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	
	camera->update();
	
	//waterTerrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), false, waterTimer/100.f);


	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	m_Terrain->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"Rock"), textureMgr->getTexture(L"Snow"), textureMgr->getTexture(L"Mud"), light);
	shader->render(renderer->getDeviceContext(), m_Terrain->getIndexCount());


	// Send geometry data, set shader parameters, render object with shader
	waterTerrain->sendData(renderer->getDeviceContext());
	XMFLOAT3 waterValues = XMFLOAT3(waterTerrain->size(), terrainResolution, water);

	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), light, waterValues);
	waterShader->render(renderer->getDeviceContext(), waterTerrain->getIndexCount());



	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);


	
	// Build UI
	
	ImGui::Text("FPS: %.2f", timer->getFPS());
	


	//Terrain doesnt start off spawned, change values to spawn terrain

	if (ImGui::Button("Regenerate Terrain")) {
		m_Terrain->Resize(terrainResolution, -ChangeY);
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), false);
	}

	if (ImGui::Button("Rescale Terrains")) {
		m_Terrain->Scale(ChangeScale);
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), false);
		waterTerrain->Scale(ChangeScale);
		waterTerrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), false, 1);
	}



	if (ImGui::Button("Smooth Terrain"))

	{

		if (newiterations != m_Terrain->getIterations()) {
			m_Terrain->setheightmap(true);
			m_Terrain->setiterations(newiterations);
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), true);
			m_Terrain->setheightmap(false);

		}

		//m_Terrain->smooth();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), true);

	}


	if (ImGui::Button("Change FBM"))
	{

		if (newamount1 != m_Terrain->Checkamount1())
		{
			m_Terrain->BuildHeightMap(newamount1, newamount2, newamount3);
		}
		if (newamount2 != m_Terrain->Checkamount2())
		{
			m_Terrain->BuildHeightMap(newamount1, newamount2, newamount3);
		}

		m_Terrain->BuildHeightMap(newamount1, newamount2, newamount3);
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext(), false);
		
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Edit Noise Functions"))
	{
		ImGui::SliderFloat("Change Falloff", &newamount1, 1, 30);
		ImGui::SliderFloat("Change Scale", &newamount2, 1, 30);
		ImGui::SliderFloat("Change Height", &newamount3, 1, 5);
	}
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Edit Terrain Values"))
	{
		ImGui::SliderInt("Terrain Resolution", &terrainResolution, 2, 1024); //change terrain resolution
		ImGui::SliderFloat("Change Y value", &ChangeY, 0, 30); //moves the full terrian up or down
		ImGui::SliderFloat("Terrain Scale", &ChangeScale, 0, 10); //Changes both terrains Scale
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Misc"))
	{
		ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	}

// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

