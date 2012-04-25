#pragma once

class HexGrid
{
private:
public: // debugging
	static const short scaleUV = 0x1000>>1; //1.0
	class CIwSVec3 *Verts3D;
	class CIwSVec2 *UVs;
	uint16 *Indices;
	uint16 width;
	uint16 height;
	int16 currentX, currentY;
	int16 offsetGridX, offsetGridY;
	// Logic part
	HexGrid();
public:
	HexGrid(uint16 _x, uint16 _y, uint16 _offsetX, uint16 _offsetY);
	static void update();
	~HexGrid();
	// Graphic part
	void render();
	static CIwSVec2 GetTopLeftPixelBoundingRectangle (int x , int y, int size);
	static void setTexture(class CIwTexture* _texture, uint16 _textureSizeX, uint16 _textureSizeY);
	float findClosest(int32 px, int32 py, int32 &closestX, int32 &closestY);
	float findClosestSimple(int32 px, int32 py, int32 &closestX, int32 &closestY);
	float findClosestArray(CIwVec3 &o, CIwVec3 &dir, int32 &closestX, int32 &closestY);
private:
	static void renderNoSetupMaterial();
	static void renderSetupMaterial();
	static void renderTriIndexed();
	void getVertexTopFlat3D(int16 x, int16 y, class CIwSVec3 *xy, class CIwSVec2* uv);
	void getIndexArray(int16 x, int16 y, uint16 *index);
	float getDistance(class CIwSVec3 *xy, int16 px, int16 py);
	float getDistanceV(class CIwVec3 &a, class CIwVec3 &n, class CIwVec3 p);
	static class CIwTexture* texture;
	static uint16 textureSizeX;
	static uint16 textureSizeY;
	uint16 offsetX;
	uint16 offsetY;
};
