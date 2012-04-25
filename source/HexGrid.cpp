#define _WIN32
#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#pragma warning (push)
#pragma warning (disable:4100 4127 4201 4512 4244)
#endif
#include "IwMaterial.h" 
#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#pragma warning (pop)
#endif
#include "HexGrid.h"
#include <math.h>

class CIwTexture* HexGrid::texture = 0;
uint16 HexGrid::textureSizeX = 0;
uint16 HexGrid::textureSizeY = 0;
const float COS30 = 0.86602540378443864676372317075294f;

HexGrid::HexGrid() { };

HexGrid::HexGrid(uint16 _x, uint16 _y, uint16 _offsetX, uint16 _offsetY) {
	//const float SIN30 = 0.5f;
	width = _x;
	height = _y;
	offsetX = _offsetX;
	offsetY = _offsetY;
	currentX = -1;
	currentY = -1;
	offsetGridX = offsetX*3/4;
	offsetGridX+=offsetGridX&1;
	offsetGridY = int16(offsetY*COS30);
	offsetGridY+=offsetGridY&1;

	int ncell = width * height;

	Verts3D = new(CIwSVec3[ncell*6]);
	UVs = new(CIwSVec2[ncell*6]);
	Indices = new(uint16[ncell*8]);
	for (int16 x = 0; x < width; x++)
		for (int16 y = 0; y < height; y++) {
			getVertexTopFlat3D(x, y, Verts3D, UVs/*, Colors3D*/);
			getIndexArray(x, y, Indices);
		}
}

HexGrid::~HexGrid(void)
{
	if (Indices != 0) {
		delete []Indices; Indices = 0;
	}
	if (UVs != 0) {
		delete []UVs; UVs = 0;
	}
	if (Verts3D != 0) {
		delete []Verts3D; Verts3D = 0;
	}
}

void HexGrid::update()
{
}

void HexGrid::setTexture(CIwTexture* _texture, uint16 _textureSizeX, uint16 _textureSizeY) {
	texture = _texture;
	texture->Upload();
	textureSizeX = _textureSizeX;
	textureSizeY = _textureSizeY;
}

void HexGrid::renderNoSetupMaterial()
{
	// limited to 200 cases
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
//	pMat->SetAlphaMode(CIwMaterial::ALPHA_NONE);
//	pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
//	pMat->SetZDepthOfs(10);
////	pMat->SetTexture(texture);
//	pMat->SetColEmissive(0);
//	pMat->SetColAmbient(255,255,255,255);
	IwGxSetMaterial(pMat);
}

void HexGrid::renderSetupMaterial()
{
	// limited to 200 cases
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
	pMat->SetZDepthOfs(10);
	pMat->SetTexture(texture);
	pMat->SetColEmissive(0);
	pMat->SetColAmbient(255,255,255,255);
	IwGxSetMaterial(pMat);
}

void HexGrid::render() {
//		IwGxSetDebugFlags(IW_GX_DEBUG_WIREFRAME_F); 
	if (texture != NULL)
	{
		int ncell = width * height;
		renderSetupMaterial();

		IwGxSetVertStreamModelSpace(Verts3D, ncell*6);

			IwGxSetColStream(NULL);
		IwGxSetUVStream(UVs);

		IwGxDrawPrims(IW_GX_TRI_STRIP, Indices, ncell * 8);
	}
}


//NOT TESTED
CIwSVec2 HexGrid::GetTopLeftPixelBoundingRectangle (int x , int y, int size)
{
	// pre set some values
	const float SIN30 = 0.5f;
	const float COS30 = 0.86602540378443864676372317075294f;
	float side = float(size); 
	float radius = side * COS30;
//	float width = radius * 2 ;
	float H  = side * SIN30;
//	float height = 2*H + side;
//	float gradient = H/radius;

	CIwSVec2 point = CIwSVec2 (
		short(x * 2 * radius + ( y & 1) * radius) ,
		short(y *  (H + side)) ); 
	return point; 
}

#if 0
//Not Finished
void HexGrid::getVertexSideFlat(int x, int y, CIwSVec2 *xy, CIwSVec2* uv) {
	const float SIN30 = 0.5f;
	const float COS30 = 0.86602540378443864676372317075294f;
	const short scaleUV = 2048;
	int vertex = (x + y * width)*6;
	xy += vertex;
	uv += vertex;

	int16 originx = width*offset_x-x*offset_x;
	int16 originy = 2*y*offset_y;
	int16 offsetx = offset_x*COS30;
	int16 offsety = offset_y*2;
	float scaleUVx = scaleUV*COS30;
	float scaleUVy = scaleUV;
	xy[0].x = short(originx-offsetx*0.0f);
	xy[0].y = short(originy+offsety*0.25f);
	uv[0].x = scaleUV+short(scaleUVx*0.0f);
	uv[0].y = short(scaleUVy*0.25f);
	xy[1].x = short(originx-offsetx/2.0f);
	xy[1].y = short(originy+offsety*0.0f);
	uv[1].x = scaleUV+short(scaleUVx/2.0f);
	uv[1].y = short(scaleUVy*0.0f);
	xy[2].x = short(originx-offsetx);
	xy[2].y = short(originy+offsety*0.25f);
	uv[2].x = scaleUV+short(scaleUVx);
	uv[2].y = short(scaleUVy*0.25f);
	xy[3].x = short(originx-offsetx);
	xy[3].y = short(originy+offsety*0.75f);
	uv[3].x = scaleUV+short(scaleUVx);
	uv[3].y = short(scaleUVy*0.75f);
	xy[4].x = short(originx-offsetx/2.0f);
	xy[4].y = short(originy+offsety*1.0f);
	uv[4].x = scaleUV+short(scaleUVx/2.0f);
	uv[4].y = short(scaleUVy*1.0f);
	xy[5].x = short(originx-offset_x*0.0f);
	xy[5].y = short(originy+offsety*0.75f);
	uv[5].x = scaleUV+short(scaleUV*0.0f);
	uv[5].y = short(scaleUVy*0.75f);
	//if (vertex == 0) {
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[0].x, xy[0].y);
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[1].x, xy[1].y);
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[2].x, xy[2].y);
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[3].x, xy[3].y);
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[4].x, xy[4].y);
	//	printf("%3d, %3d:(%3d, %3d)\n", x, y, xy[5].x, xy[5].y);
	//}
}
#endif

void HexGrid::getVertexTopFlat3D(int16 x, int16 y, CIwSVec3 *xy, CIwSVec2* uv) {
//--2---3
//1------4
//--0---5
	short originUVx = 0;
	short originUVy = 0;
	int vertex = (x + y * width)*6;
	xy += vertex;
	uv += vertex;

	int16 originx = offsetX+x*offsetGridX;// - width*offsetGridX/2;
	int16 originy = y*offsetGridY;// - height*offsetGridY/2;
	if (x&1)
		originy += offsetGridY/2;
	int loffsetX = offsetX;
	int loffsetY = offsetGridY;
	if ((x+y)%3 == 0) {
		originUVx = 0x1000>>1;
		originUVy = 0;
	} else if ((x+y)%3 == 1) {
		originUVx = 0;
		originUVy = 0x1000>>1;
	} else if ((x+y)%3 == 2) {
		originUVx = 0x1000>>1;
		originUVy = 0x1000>>1;
	}
	xy[0].x = short(originx-loffsetX*3/4);
	xy[0].y = short(originy+loffsetY*0);
	xy[0].z = 0;
	uv[0].x = originUVx+short(scaleUV*3/4);
	uv[0].y = originUVy+short(scaleUV*0);
	xy[1].x = short(originx-loffsetX);
	xy[1].y = short(originy+loffsetY/2);
	xy[1].z = 0;
	uv[1].x = originUVx+short(scaleUV);
	uv[1].y = originUVy+short(scaleUV/2);
	xy[2].x = short(originx-loffsetX*3/4);
	xy[2].y = short(originy+loffsetY);
	xy[2].z = 0;
	uv[2].x = originUVx+short(scaleUV*3/4);
	uv[2].y = originUVy+short(scaleUV);
	xy[3].x = short(originx-loffsetX*1/4);
	xy[3].y = short(originy+loffsetY);
	xy[3].z = 0;
	uv[3].x = originUVx+short(scaleUV*1/4);
	uv[3].y = originUVy+short(scaleUV);
	xy[4].x = short(originx-loffsetX*0);
	xy[4].y = short(originy+loffsetY/2);
	xy[4].z = 0;
	uv[4].x = originUVx+short(scaleUV*0);
	uv[4].y = originUVy+short(scaleUV/2);
	xy[5].x = short(originx-loffsetX/4);
	xy[5].y = short(originy+loffsetY*0);
	xy[5].z = 0;
	uv[5].x = originUVx+short(scaleUV/4);
	uv[5].y = originUVy+short(scaleUV*0);
}

void HexGrid::getIndexArray(int16 x, int16 y, uint16 *index) {
	short vertex = (x + y * width)*6;
	index += (x + y * width)*8;
	index[0] = vertex; // 012
	index[1] = vertex;
	index[2] = vertex + 5;
	index[3] = vertex + 1 ; //025
    index[4] = vertex + 4;
    index[5] = vertex + 2;
    index[6] = vertex + 3; // 523
    index[7] = vertex + 3;
}

float HexGrid::getDistance(class CIwSVec3 *xy, int16 px, int16 py) {
	int centerX=(xy[1].x+xy[4].x)/2;
	int centerY=(xy[1].y+xy[4].y)/2;
	return float(sqrt((centerX-px)*(centerX-px)+(centerY-py)*(centerY-py)));
}

float HexGrid::getDistanceV(class CIwVec3 &a, class CIwVec3 &n, class CIwVec3 p) {
	CIwVec3 line1;
	line1 = (a - p);
	int lenline1 = line1.Dot(n);
	CIwVec3 v = (line1 - (lenline1 * n));
	return float(sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
}

float HexGrid::findClosestSimple(int32 px, int32 py, int32 &closestX, int32 &closestY) {
	float closestDistance = 9999999.0f;
	for (int16 x = 0; x < width; x++) {
		for (int16 y = 0; y < height; y++) {
			int vertex = (x + y * width)*6;
			CIwSVec3 *xy=&Verts3D[vertex];
			float distance = getDistance(xy, int16(px), int16(py));
			if (distance < closestDistance) {
				closestDistance = distance;
				closestX = x;
				closestY = y;
			}
		}
	}
	return closestDistance;
}

float HexGrid::findClosestArray(CIwVec3 &o, CIwVec3 &dir, int32 &closestX, int32 &closestY) {
	float closestDistance = 9999999.0f;
	for (int16 x = 0; x < width; x++) {
		for (int16 y = 0; y < height; y++) {
			int vertex = (x + y * width)*6;
			CIwSVec3 *xy=&Verts3D[vertex];
			int centerX=(xy[1].x+xy[4].x)/2;
			int centerY=(xy[1].y+xy[4].y)/2;
//			int radius = int((xy[4].x-xy[1].x)/2 * COS30);

			float distance = getDistanceV(o, dir, CIwVec3(centerX, centerY, 0));
			if (distance < closestDistance) {
				closestDistance = distance;
				closestX = x;
				closestY = y;
			}
		}
	}
	return closestDistance;
}

