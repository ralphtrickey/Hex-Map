#pragma once
enum eMouseMode {MOUSE_MODE_IDLE, MOUSE_MODE_CHECKING, MOUSE_MODE_DOWN};
class HexMapTest {
private:
	static const uint16 hexGridMaxX=50, hexGridMaxY=50, textureMaxX=64, textureMaxY=64;

	static enum eMouseMode mouse_mode;
    CIwVec3 s_up, s_left, s_Angles;
    CIwVec3 s_cameraPos;
    // Initialise view matrix
    CIwMat s_viewMatrix, s_ModelMatrix;
    CIwMat s_viewMatrix_initial, s_ModelMatrix_initial;
    CIwTexture *s_BitMapTexture;
	CIwResGroup *pGroup;
	class HexGrid *hexGrid;
	bool zooming, rotating;
	float zoom, rotation;
	float zoom_initial, rotation_initial;
	int16 translationX, translationY;
	int16 translationX_initial, translationY_initial;
	CIwVec3 world_initial;

	static bool KeyPressed(s3eKey key);
	static bool KeyAltDown();
	static bool KeyShiftDown();
	static bool KeyControlDown();
	bool UpdateKey();
	void updateLookAt(CIwVec3 &lookFrom);
	CIwVec3 getWorldCoords(int x, int y);
	static CIwVec3 getIntersectionNew(CIwVec3 &o, CIwVec3 &dir);
	void DebugPrint(int closestX, int closestY);
	void SetTranslation();
	void SetRotation(float deltaRotation);
	void SetZoom(float deltaZoom);
	void SetModelMatrix();
public:
	void Init(void);
	bool Update(void);
	void Render(void);
	void ShutDown(void);
};

