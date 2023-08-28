#include "TerrainMesh1.h"
#include "PerlinNoise.h"
TerrainMesh1::TerrainMesh1(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution, float ChangeY, float ChangeScale) :
	PlaneMesh(device, deviceContext, lresolution)
{
	Resize(resolution, ChangeY);
	Scale(ChangeScale);
	Regenerate(device, deviceContext, false);
}

//Cleanup the heightMap
TerrainMesh1::~TerrainMesh1() {
	for (int i = 0; i < heightMap.size(); i++)
	{
		heightMap[i].clear();
	}
}


//Fill an array of floats that represent the height values at each grid point.
//Here we are producing a Sine wave along the X-axis
void TerrainMesh1::BuildHeightMap(float newamount1, float newamount2, float newamount3) {
	float height = 0;
	//this is code of mine from another slider
	float height2 = 0.0f;
	amount1 = newamount1;
	amount2 = newamount2;
	amount3 = newamount3;


	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			heightMap[i][j] = 10; //sets the height of the terrain
		}
	}

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;
	float maxHeight = 5;
	//pow is base to the power of the exponent
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			for (int k = 0; k < 10; k++)
			{

				heightMap[j][i] += (1 - std::abs(PerlinNoise::PerlinNoise::noise((i * std::pow(amount1, k) * scale) / (amount2), (j * std::pow(amount1, k) * scale) / (amount2)))) * (amount1 / std::pow(amount1, k)); //this line defines how many perlin noise is loaded on top of each other using FBM to create something more realistic
																																																						//pow function thats being used here is to control the smoothness and roughness of the noise		
				heightMap[j][i] += PerlinNoise::PerlinNoise::noise((i * std::pow(amount1, k) * scale) / (amount2), (j * std::pow(amount1, k) * scale) / (amount2)) * (amount1 / std::pow(amount1, k));
			}
			heightMap[j][i] += 1; //setting the position of the map
			heightMap[j][i] += changeY; //Changing the Y value of the terrain
			

			if (heightMap[j][i] > maxHeight) {
				maxHeight = heightMap[j][i];
			}
		}

	}
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			heightMap[j][i] -= maxHeight;
			heightMap[j][i] *= -1; //sets maximum negative height/minimum height
			heightMap[j][i] -= maxHeight / 2;
			heightMap[j][i] *= amount3; //sets maximum positive height

			for (int k = 2; k < 4; k++)
			{

				heightMap[j][i] += PerlinNoise::PerlinNoise::noise((i * std::pow(amount1, k) * scale) / (amount2), (j * std::pow(amount1, k) * scale) / (amount2)) * (amount1 / std::pow(amount1, k));
			}
		}
	}
}

void TerrainMesh1::Resize(int newResolution, float ChangeY) {
	changeY = ChangeY;
	resolution = newResolution;
	heightMap.resize(newResolution);
	for (int i = 0; i < heightMap.size(); i++)
	{
		heightMap[i].resize(resolution);
	}
	if (vertexBuffer != NULL) {
		vertexBuffer->Release();
	}
	vertexBuffer = NULL;
}


void TerrainMesh1::Scale(float ChangeScale)
{
	changeScale = ChangeScale;
	terrainSize += changeScale;
}


// Set up the heightmap and create or update the appropriate buffers
void TerrainMesh1::Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, bool smoothy) {

	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, height, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Calculate and store the height values

	perlinNoise(amount1, amount2);

	if (!smoothy)
	{
		BuildHeightMap(amount1, amount2, amount3);
	}


	if (heightchangeon == true) {
		
		perlinNoise(amount1, amount2);
	}

	//smooth function

	for (int i = 0; i < iterations; i++)
	{
		smooth();
	}
	


	// Calculate the number of vertices in the terrain mesh.

	vertexCount = resolution * resolution;

	indexCount = ((resolution - 1) * (resolution - 1)) * 6;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	index = 0;

	// UV coords.
	u = 0;
	v = 0;
	increment = m_UVscale / resolution;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;


	//Set up vertices
	for (j = 0; j < (resolution); j++) {
		for (i = 0; i < (resolution); i++) {
			positionX = (float)i * scale;
			positionZ = (float)(j)*scale;

			height = heightMap[j][i];
			vertices[index].position = XMFLOAT3(positionX, height, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
	}

	//Set up index list
	index = 0;
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {

			//Build index array
			indices[index] = (j * resolution) + i;
			indices[index + 1] = ((j + 1) * resolution) + (i + 1);
			indices[index + 2] = ((j + 1) * resolution) + i;

			indices[index + 3] = (j * resolution) + i;
			indices[index + 4] = (j * resolution) + (i + 1);
			indices[index + 5] = ((j + 1) * resolution) + (i + 1);
			index += 6;
		}
	}

	//Set up normals
	for (j = 0; j < (resolution - 1); j++) {
		for (i = 0; i < (resolution - 1); i++) {
			//Calculate the plane normals
			XMFLOAT3 a, b, c;	//Three corner vertices
			a = vertices[j * resolution + i].position;
			b = vertices[j * resolution + i + 1].position;
			c = vertices[(j + 1) * resolution + i].position;

			//Two edges
			XMFLOAT3 ab(c.x - a.x, c.y - a.y, c.z - a.z);
			XMFLOAT3 ac(b.x - a.x, b.y - a.y, b.z - a.z);

			//Calculate the cross product
			XMFLOAT3 cross;
			cross.x = ab.y * ac.z - ab.z * ac.y;
			cross.y = ab.z * ac.x - ab.x * ac.z;
			cross.z = ab.x * ac.y - ab.y * ac.x;
			float mag = (cross.x * cross.x) + (cross.y * cross.y) + (cross.z * cross.z);
			mag = sqrtf(mag);
			cross.x /= mag;
			cross.y /= mag;
			cross.z /= mag;
			vertices[j * resolution + i].normal = cross;
		}
	}

	//Smooth normals by averaging all the normals from the surrounding plane
	XMFLOAT3 smoothNorm(0, 1, 0);
	for (j = 0; j < resolution; j++) {
		for (i = 0; i < resolution; i++) {
			smoothNorm.x = 0;
			smoothNorm.y = 0;
			smoothNorm.z = 0;
			float count = 0;
			//Left planes
			if ((i - 1) >= 0) {
				//Top planes
				if ((j) < (resolution - 1)) {
					smoothNorm.x += vertices[j * resolution + (i - 1)].normal.x;
					smoothNorm.y += vertices[j * resolution + (i - 1)].normal.y;
					smoothNorm.z += vertices[j * resolution + (i - 1)].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothNorm.x += vertices[(j - 1) * resolution + (i - 1)].normal.x;
					smoothNorm.y += vertices[(j - 1) * resolution + (i - 1)].normal.y;
					smoothNorm.z += vertices[(j - 1) * resolution + (i - 1)].normal.z;
					count++;
				}
			}
			//right planes
			if ((i) < (resolution - 1)) {

				//Top planes
				if ((j) < (resolution - 1)) {
					smoothNorm.x += vertices[j * resolution + i].normal.x;
					smoothNorm.y += vertices[j * resolution + i].normal.y;
					smoothNorm.z += vertices[j * resolution + i].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothNorm.x += vertices[(j - 1) * resolution + i].normal.x;
					smoothNorm.y += vertices[(j - 1) * resolution + i].normal.y;
					smoothNorm.z += vertices[(j - 1) * resolution + i].normal.z;
					count++;
				}
			}
			smoothNorm.x /= count;
			smoothNorm.y /= count;
			smoothNorm.z /= count;

			float mag = sqrt((smoothNorm.x * smoothNorm.x) + (smoothNorm.y * smoothNorm.y) + (smoothNorm.z * smoothNorm.z));
			smoothNorm.x /= mag;
			smoothNorm.y /= mag;
			smoothNorm.z /= mag;

			vertices[j * resolution + i].normal = smoothNorm;
		}
	}
	//If we've not yet created our dyanmic Vertex and Index buffers, do that now
	if (vertexBuffer == NULL) {
		CreateBuffers(device, vertices, indices);
	}
	else {
		//If we've already made our buffers, update the information
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//  Disable GPU access to the vertex buffer data.
		deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, vertices, sizeof(VertexType) * vertexCount);
		//  Reenable GPU access to the vertex buffer data.
		deviceContext->Unmap(vertexBuffer, 0);
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}



float TerrainMesh1::length(Line line)
{
	float length;
	float x = line.end.x - line.start.x;
	float y = line.end.y - line.start.y;
	length = sqrt((x * x) + (y * y));

	return length;
}


void TerrainMesh1::fault() {
	vector<vector<float>> NewheightMap;
	NewheightMap = heightMap;

	Vector3f startPoint, endPoint;
	Line FaultLine;

	startPoint.x = 0;
	startPoint.y = rand() % resolution;


	endPoint.x = resolution - 1;
	endPoint.y = rand() % resolution;

	FaultLine.start = startPoint;
	FaultLine.end = endPoint;

	for (int i = 0; i < heightMap.size(); i++)
	{
		for (int j = 0; j < heightMap[i].size(); j++)
		{
			Vector3f Currentpoint;
			Currentpoint.x = i;
			Currentpoint.y = j;

			Line PointLine;
			PointLine.start = startPoint;
			PointLine.end = Currentpoint;


		}
	}


}

void TerrainMesh1::smooth()
{
	vector<vector<float>> NewheightMap;
	NewheightMap = heightMap;

	for (int i = 0; i < heightMap.size(); i++)
	{
		for (int j = 0; j < heightMap[i].size(); j++)
		{
			float heightSum = 0;
			int heightTally = 0;
			if (i == 0) {
				heightSum += heightMap[i + 1][j];
				heightTally++;
			}
			else if (i == heightMap.size() - 1) {
				heightSum += heightMap[i - 1][j];
				heightTally++;
			}
			else {
				heightSum += heightMap[i - 1][j];
				heightSum += heightMap[i + 1][j];
				heightTally += 2;
			}

			if (j == 0) {
				heightSum += heightMap[i][j + 1];
				heightTally++;
			}
			else if (j == heightMap.size() - 1) {
				heightSum += heightMap[i][j - 1];
				heightTally++;
			}
			else {
				heightSum += heightMap[i][j + 1];
				heightSum += heightMap[i][j - 1];
				heightTally += 2;
			}
			NewheightMap[i][j] = heightSum / heightTally;
		}
	}
	heightMap = NewheightMap;
}
void TerrainMesh1::perlinNoise(float newamount1, float newamount2)
{
	amount1 = newamount1;
	amount2 = newamount2;
	float scaling = 0.5;
	float height = 0.0;
	for (int j = 0; j < (resolution); j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			height = PerlinNoise::PerlinNoise::noise(double(i * scaling * amount2), double(j * scaling * amount2)) * amount1;
			//heightMap[(j * resolution) + i] = height;
		}
	}


}

//Create the vertex and index buffers that will be passed along to the graphics card for rendering
//For CMP305, you don't need to worry so much about how or why yet, but notice the Vertex buffer is DYNAMIC here as we are changing the values often
void TerrainMesh1::CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices) {

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}