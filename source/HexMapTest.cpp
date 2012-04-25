/*
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * Copyright Ralph Trickey.
 * I'm releasing it under MS-PL.
 * Inspiration came from a number of sources including
 * HexEngine - Ben Kloosterman - C# program
 * Hexplanet -Joel Davis - Interesting method of tiling a planet.
 * HexGrid - Unknown - It does a basic offset block grid.
 */

#define _WIN32
#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#pragma warning (push)
#pragma warning (disable:4100 4127 4201 4512 4244 4673 4670)
#endif
#include "s3e.h"
#include "IwGx.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#pragma warning (pop)
//#pragma warning (disable:4127)
#endif
#include "HexMapTest.h"
#include "HexGrid.h"
#include "CInput.h"

const int SCREEN_MARGIN = 40;
enum eMouseMode HexMapTest::mouse_mode = MOUSE_MODE_IDLE;

CIwTexture* IW_TEXTURE(CIwResGroup *group, const char *name) { return ((CIwTexture*)(group)->GetResNamed((name), IW_GX_RESTYPE_TEXTURE)); }

const CIwImage::Format TEXTURE_FORMAT = CIwImage::RGB_565;

HexMapTest hexMapTest;

void ExampleInit() {
	hexMapTest.Init();
}

//-----------------------------------------------------------------------------
void HexMapTest::Init()
{
	zoom = 1.0f;
	rotation = 0.0f;
	translationX = 0;
	translationY = 0;

	// Initialise the input system
	g_Input.Init();

    // Initialise vectors
    s_left      =  CIwVec3::g_AxisX;
    s_up        = -CIwVec3::g_AxisY;
    s_Angles = CIwSVec3::g_Zero;
    s_cameraPos =  CIwVec3(0, 0, -0x80);

    // Initialise view matrix
    s_viewMatrix = CIwMat::g_Identity;

    s_viewMatrix.SetTrans(s_cameraPos);
    s_viewMatrix.LookAt(
        s_viewMatrix.GetTrans(),
         CIwVec3::g_Zero,
        -CIwVec3::g_AxisY);

	// Initialise
    IwGxInit();
	IwResManagerInit();
    // Set screen clear colour
    IwGxSetColClear(0x40, 0x40, 0x40, 0x00);

    // Turn all lighting off
    IwGxLightingOff();

    // Set field of view
    IwGxSetPerspMul(0xa0);

    // Set near and far planes
    IwGxSetFarZNearZ(0x1000, 0x10);

    // Create empty texture object
    s_BitMapTexture = new CIwTexture;

    // Load image data from disk into texture
    s_BitMapTexture->LoadFromFile("./textures/testTexture_8bit.bmp");

    // "Upload" texture to VRAM
    s_BitMapTexture->Upload();


    // Set the view matrix along the -ve z axis
    CIwMat view = CIwMat::g_Identity;
    view.t.z = -0x100;
    IwGxSetViewMatrix(&view);
	IwGetResManager()->LoadGroup("TileRes.group"); 
	pGroup = IwGetResManager()->GetGroupNamed("TileRes");
	hexGrid = new HexGrid(hexGridMaxX, hexGridMaxY, textureMaxX, textureMaxY);
	hexGrid->setTexture(IW_TEXTURE(pGroup,"grid"),textureMaxX,textureMaxY);
	s_ModelMatrix.SetIdentity();
}

void ExampleShutDown() {
	hexMapTest.ShutDown();
}
//-----------------------------------------------------------------------------
void HexMapTest::ShutDown() {
    // Destroy texture
    delete s_BitMapTexture;

	// Terminate
	delete hexGrid;
	IwGetResManager()->DestroyGroup(pGroup);
	IwResManagerTerminate();
    IwGxTerminate();
}
void HexMapTest::updateLookAt(CIwVec3 &lookFrom) {
	s_viewMatrix.SetTrans(lookFrom);
	s_viewMatrix.LookAt(
		s_viewMatrix.GetTrans(),
		CIwVec3::g_Zero,
		s_viewMatrix.RotateVec(-s_up));
}
bool HexMapTest::KeyPressed(s3eKey key) {
	return ((s3eKeyboardGetState(key) & S3E_KEY_STATE_DOWN) != 0);
}
bool HexMapTest::KeyAltDown() {
	return ((KeyPressed(s3eKeyLeftAlt)) ||
			(KeyPressed(s3eKeyRightAlt)));
}
bool HexMapTest::KeyShiftDown() {
	return ((KeyPressed(s3eKeyLeftShift)) ||
			(KeyPressed(s3eKeyRightShift)));
}
bool HexMapTest::KeyControlDown() {
	return ((KeyPressed(s3eKeyLeftControl)) ||
			(KeyPressed(s3eKeyRightControl)));
}

bool ExampleUpdate() {
	return hexMapTest.Update();
}

bool HexMapTest::UpdateKey() {
	CIwMat   rotation;
    CIwVec3  lookFrom(0,0,0);

	if (KeyPressed(s3eKeyPageUp)) {
		if (KeyAltDown()) {
			rotation.SetAxisAngle(
				s_viewMatrix.RotateVec(s_left),
				IW_ANGLE_FROM_DEGREES(0));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[2] -= 0x010;
			if (s_cameraPos.z<-0x40)
				s_cameraPos.z += 0x40;
		}
	}

	if (KeyPressed(s3eKeyPageDown))
	{
		if (KeyAltDown()) {
			s_cameraPos.z -= 0x40;
			rotation.SetAxisAngle(
				s_viewMatrix.RotateVec(s_left),
				IW_ANGLE_FROM_DEGREES(0));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[2] += 0x010;
		}
	}

    // Up, Down, Left, Right keys
    //

    if (KeyPressed(s3eKeyUp))
    {
		if (KeyAltDown()) {
			rotation.SetAxisAngle(
				s_viewMatrix.RotateVec(s_left),
				IW_ANGLE_FROM_DEGREES(-5));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[1] -= 0x010;
		}
    }

    if (KeyPressed(s3eKeyDown))
    {
		if (KeyAltDown()) {
			rotation.SetAxisAngle(
				s_viewMatrix.RotateVec(s_left),
				IW_ANGLE_FROM_DEGREES(5));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[1] += 0x010;
		}
    }

    if (KeyPressed(s3eKeyRight))
    {
		if (KeyAltDown()) {
			rotation.SetAxisAngle(
				s_viewMatrix.TransformVec(s_up),
				IW_ANGLE_FROM_DEGREES(5));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[0] += 0x010;
		}
    }

	if (KeyPressed(s3eKeyLeft))
    {
		if (KeyAltDown()) {
			rotation.SetAxisAngle(
				s_viewMatrix.TransformVec(s_up),
				IW_ANGLE_FROM_DEGREES(-5));
			lookFrom = (s_viewMatrix * rotation).RotateVec(
				s_cameraPos);
			updateLookAt(lookFrom);
		} else {
			s_viewMatrix.t[0] -= 0x010;
		}
    }
	return true;
}
void HexMapTest::SetZoom(float deltaZoom, int multiplier)
{
	if (deltaZoom >= 1)
		deltaZoom = (deltaZoom-1)*multiplier+1;
	else
		deltaZoom = (deltaZoom-1)+1;
	if (multiplier != 1 && deltaZoom > float(multiplier))
		deltaZoom = float(multiplier);
	if (multiplier != 1 && deltaZoom < 1.0f/multiplier)
		deltaZoom = 1.0f/multiplier;
	zoom = zoom_initial * deltaZoom;
	SetModelMatrix();
}
void HexMapTest::SetRotation(float deltaRotation)
{
	rotation = rotation_initial + deltaRotation;
	while (rotation >= 360.0f)
		rotation -= 360.0f;
	while (rotation < 0.0f)
		rotation += 360.0f;
	SetModelMatrix();
}
void HexMapTest::SetTranslation()
{
	int16 sprite1_pos_x,sprite1_pos_y,sprite1_pos_x_initial,sprite1_pos_y_initial;
	if (g_Input.finger1MovedTo(sprite1_pos_x, sprite1_pos_y)) {
		g_Input.finger1Initial(sprite1_pos_x_initial, sprite1_pos_y_initial);
	}
	iwfixed sinTheta = IwGeomSin(IW_ANGLE_FROM_DEGREES(rotation));
	iwfixed cosTheta = IwGeomCos(IW_ANGLE_FROM_DEGREES(rotation));

	int odx = sprite1_pos_x_initial - sprite1_pos_x;
	int ody = sprite1_pos_y_initial - sprite1_pos_y;

	int dx = (IW_FIXED_MUL(odx, cosTheta) - IW_FIXED_MUL(ody, sinTheta));
	int dy = (IW_FIXED_MUL(odx, sinTheta) + IW_FIXED_MUL(ody, cosTheta));

	translationX = translationX_initial + dx;
	translationY = translationY_initial + dy;
	SetModelMatrix();
}
void HexMapTest::SetModelMatrix() {
	s_ModelMatrix.SetIdentity();
	CIwVec3 vectCenter = getWorldCoords(IwGxGetScreenWidth()/2, IwGxGetScreenHeight()/2);
	s_ModelMatrix.t.x = vectCenter.x;
	s_ModelMatrix.t.y = vectCenter.y;

	//Zoom
	s_ModelMatrix.Scale(IW_FIXED_FROM_FLOAT(zoom));
	CIwVec3 vect = getWorldCoords(0, 0);
	CIwVec3 vect2 = getWorldCoords(translationX, translationY);

	iwfixed dx = IW_FIXED_MUL(vect2.x-vect.x, IW_FIXED_FROM_FLOAT(zoom));
	iwfixed dy = IW_FIXED_MUL(vect2.y-vect.y, IW_FIXED_FROM_FLOAT(zoom));

	//s_ModelMatrix.t.x = -dx;
	//s_ModelMatrix.t.y = -dy;

	iwfixed sinTheta = IwGeomSin(IW_ANGLE_FROM_DEGREES(-rotation));
	iwfixed cosTheta = IwGeomCos(IW_ANGLE_FROM_DEGREES(-rotation));

	s_ModelMatrix.t.x = -vectCenter.x-(IW_FIXED_MUL(dx, cosTheta) - IW_FIXED_MUL(dy, sinTheta));
	s_ModelMatrix.t.y = -vectCenter.y-(IW_FIXED_MUL(dx, sinTheta) + IW_FIXED_MUL(dy, cosTheta));
	//Rotate
	CIwMat rotationMat = CIwMat::g_Identity;
	rotationMat.SetRotZ(IW_ANGLE_FROM_DEGREES(rotation), true, true);
	s_ModelMatrix *= rotationMat;

}
//-----------------------------------------------------------------------------
bool HexMapTest::Update()
{
	int16 sprite1_pos_x,sprite1_pos_y;
	int16 sprite2_pos_x,sprite2_pos_y;
	if (mouse_mode == MOUSE_MODE_CHECKING)
		mouse_mode  = MOUSE_MODE_DOWN;

	// UI processing
	if (((s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED)) && mouse_mode == MOUSE_MODE_IDLE)
		mouse_mode = MOUSE_MODE_IDLE;
	if ((s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_RELEASED))
		mouse_mode = MOUSE_MODE_CHECKING;

//	UpdateKey();

	IwGxSetViewMatrix(&s_viewMatrix);

    // Generate a ray pointing to the view plane from the camera
    CIwVec3 dir(s3ePointerGetX() - IwGxGetScreenWidth()/2, 
        s3ePointerGetY() - IwGxGetScreenHeight()/2, 
        IwGxGetPerspMul());
    
    // Rotate into camera space
    dir = s_viewMatrix.RotateVec(dir);

	// Update pointer system
	g_Input.Update();
	if (g_Input.finger1IsDown()) {
		if (g_Input.overThreshold()) {
			if (!g_Input.finger1Continuing()) {
				s_ModelMatrix_initial = s_ModelMatrix;
				CIwVec3 vectCenter = getWorldCoords(0, 0);
				zoom_initial = zoom;
				rotation_initial = rotation;
				translationX_initial = translationX;
				translationY_initial = translationY;
				if (!g_Input.isMultiTouch()) {
					if (g_Input.finger1MovedTo(sprite1_pos_x, sprite1_pos_y)) {
						if (sprite1_pos_x > int(IwGxGetScreenWidth()) - SCREEN_MARGIN) {
							zooming = true;
						}
						if (sprite1_pos_y > int(IwGxGetScreenHeight()) - SCREEN_MARGIN) {
							rotating = true;
						}
					}
				}
			}
			if (!g_Input.finger2IsDown() && g_Input.finger1MovedTo(sprite1_pos_x, sprite1_pos_y)) {
				if (g_Input.finger1Continuing()) {
					if (zooming) {
						if (sprite1_pos_x > int(IwGxGetScreenWidth()) - SCREEN_MARGIN) {
							int16 dsprite1_pos_x, dsprite1_pos_y;
							if (g_Input.finger1MovementDelta(dsprite1_pos_x, dsprite1_pos_y)) {
								float deltaZoom = 1.0f - 1.0f*(dsprite1_pos_y)/IwGxGetScreenHeight();
								SetZoom(deltaZoom, 8);
							}
						}
					} else if (rotating) {
						if (sprite1_pos_y > int(IwGxGetScreenHeight()) - SCREEN_MARGIN) {
							int16 dsprite1_pos_x, dsprite1_pos_y;
							if (g_Input.finger1MovementDelta(dsprite1_pos_x, dsprite1_pos_y)) {
								float deltaRotation = 360.0f*(dsprite1_pos_x)/IwGxGetScreenWidth();
								SetRotation(deltaRotation);
							}
						}
					} else {
						SetTranslation();
					}
				}
			} else {
				if (g_Input.finger1MovedTo(sprite1_pos_x, sprite1_pos_y)) {
					if (g_Input.finger2MovedTo(sprite2_pos_x, sprite2_pos_y)) {
						if (g_Input.finger2Continuing()) {
							int16 sprite1_pos_x_initial,sprite1_pos_y_initial,sprite2_pos_x_initial,sprite2_pos_y_initial;
							g_Input.finger1Initial(sprite1_pos_x_initial,sprite1_pos_y_initial);
							g_Input.finger2Initial(sprite2_pos_x_initial,sprite2_pos_y_initial);
							int d12x=sprite1_pos_x-sprite2_pos_x;
							int d12y=sprite1_pos_y-sprite2_pos_y;
							int d12x_initial=sprite1_pos_x_initial-sprite2_pos_x_initial;
							int d12y_initial=sprite1_pos_y_initial-sprite2_pos_y_initial;
							int Delta_initial = d12x_initial*d12x_initial+d12y_initial*d12y_initial;
							int Delta = d12x*d12x+d12y*d12y;
							float newZoom = float(sqrt(1.0*Delta)/sqrt(Delta_initial));
							SetZoom(newZoom, 1);
							float oldRotation = float(atan2(float(d12y_initial), float(d12x_initial)));
							float newRotation = float(atan2(float(d12y), float(d12x)));
							SetRotation(180.0f*(oldRotation-newRotation)/PI);
							{
								char string[256];
								//sprintf(string, "`1`a  del %04d, %04d",d1x,d2x);
								//IwGxPrintString(2, 96, string);
								sprintf(string, "`1`a  zoo %5.2f, %5.2f, %5.2f",newZoom,oldRotation,newRotation);
								IwGxPrintString(2, 96, string);
							}
						}
					}
				}
			}
		}
	} else {
		zooming = false;
		rotating = false;
		//		s_ModelMatrix = CIwMat::g_Identity;
	}
	return true;
}

void ExampleRender() {
	hexMapTest.Render();
}

CIwVec3 HexMapTest::getIntersectionNew(CIwVec3 &o, CIwVec3 &dir) {
	CIwVec3 &l = dir;
	CIwVec3 &l0 = o;
	CIwVec3 n(0,0,IW_FIXED(1));
	int demoninator = l.Dot(n);
	if (demoninator == 0)
		return CIwVec3::g_Zero;
	CIwVec3 p0(IW_FIXED(1),IW_FIXED(1),0);
	int numerator=(p0-l0).Dot(n);
	float f = float(1.0*numerator/demoninator);
	return IW_FIXED_FROM_FLOAT(f)*l+l0;
}

CIwVec3 HexMapTest::getWorldCoords(int x, int y) {
// Generate a ray pointing to the view plane from the camera
	CIwVec3 dir(x - IwGxGetScreenWidth()/2, 
		y - IwGxGetScreenHeight()/2, 
		IwGxGetPerspMul());
	CIwVec3 origin = s_viewMatrix.t;
    
	// Rotate into camera space
	dir = s_viewMatrix.RotateVec(dir);

//		dir.NormaliseSlow();
	CIwVec3 inter = getIntersectionNew(origin, dir);
	inter = inter - s_ModelMatrix.t;
	iwfixed z = IW_FIXED_FROM_FLOAT(1.0f/zoom);
	inter.x = IW_FIXED_MUL(inter.x, z);
	inter.y = IW_FIXED_MUL(inter.y, z);
	inter.z = IW_FIXED_MUL(inter.z, z);

	CIwMat rotationMat = CIwMat::g_Identity;
	rotationMat.SetRotZ(IW_ANGLE_FROM_DEGREES(-rotation), true, true);
	inter = rotationMat.RotateVec(inter);
	return inter;
}
//-----------------------------------------------------------------------------
void HexMapTest::Render()
{
//	if (mouse_mode == MOUSE_MODE_CHECKING) 
	{
//		int32 px = IwGxGetScreenWidth() - s3ePointerGetX();
//		int32 py = IwGxGetScreenHeight() - s3ePointerGetY();
		int32 closestX = -1, closestY = -1;

		CIwVec3 vect = getWorldCoords(s3ePointerGetX(), s3ePointerGetY());
//		hexGrid->findClosestArray(origin, dir, closestX, closestY);
		//WORKING!!!
		hexGrid->findClosestSimple(vect.x, vect.y, closestX, closestY);
		DebugPrint(closestX, closestY);
	}
//		s_PickSurface->MakeCurrent();
    // Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
	// Set the model matrix
    IwGxSetModelMatrix(&s_ModelMatrix);
	hexGrid->render();


    // End drawing
    IwGxFlush();

	IwGxPrintSetScale(2);
	IwGxPrintFrameRate(0, 0); 
	// Swap buffers
	IwGxSwapBuffers();
}

void HexMapTest::DebugPrint(int closestX, int closestY) {
	char string [180];
	//sprintf(string, "`1`a  1 %03d, %03d",sprite1_pos_x_initial, sprite1_pos_y_initial);
	//IwGxPrintString(2, 16, string);
	//sprintf(string, "`1`a  2 %03d, %03d",sprite1_pos_x, sprite1_pos_y);
	//IwGxPrintString(2, 32, string);
	//sprintf(string, "`1`a  c %03d", g_Input.getTouchCount());
	//IwGxPrintString(2, 48, string);
	//if (g_Input.getTouchCount() > 0) {
	//	CIwVec3 vect = getWorldCoords(sprite1_pos_x_initial, sprite1_pos_y_initial);
	//	sprintf(string, "`1`a  w1 %03d, %03d", vect.x, vect.y);
	//	IwGxPrintString(2, 64, string);
	//	CIwVec3 vect2 = getWorldCoords(sprite1_pos_x, sprite1_pos_y);
	//	sprintf(string, "`1`a  w2 %03d, %03d", vect2.x, vect2.y);
	//	IwGxPrintString(2, 80, string);
	//}
	//sprintf(string, "`1`a  o%03d, %03d",origin.x,origin.y);
	//IwGxPrintString(2, 38, string);
	//sprintf(string, "`1`a  d%03d, %03d, %03d",dir.x,dir.y,dir.z);
	//IwGxPrintString(2, 54, string);
	sprintf(string, "`1`a  hex %03d, %03d",closestX, closestY);
	IwGxPrintString(2, 16, string);
	//{
	//	int16 sprite1_pos_x,sprite1_pos_y,sprite1_pos_x_initial,sprite1_pos_y_initial;
	//	if (g_Input.finger1MovedTo(sprite1_pos_x,sprite1_pos_y)) {
	//		g_Input.finger1Initial(sprite1_pos_x_initial,sprite1_pos_y_initial);
	//		sprintf(string, "`1`a  ini %04d, %04d",sprite1_pos_x_initial,sprite1_pos_y_initial);
	//		IwGxPrintString(2, 32, string);
	//		sprintf(string, "`1`a  cur %04d, %04d",sprite1_pos_x,sprite1_pos_y);
	//		IwGxPrintString(2, 48, string);
	//	}
	//}
	//{
	//	int16 sprite2_pos_x,sprite2_pos_y,sprite2_pos_x_initial,sprite2_pos_y_initial;
	//	if (g_Input.finger2MovedTo(sprite2_pos_x,sprite2_pos_y)) {
	//		g_Input.finger2Initial(sprite2_pos_x_initial,sprite2_pos_y_initial);
	//		sprintf(string, "`1`a  ini %04d, %04d",sprite2_pos_x_initial,sprite2_pos_y_initial);
	//		IwGxPrintString(2, 64, string);
	//		sprintf(string, "`1`a  cur %04d, %04d",sprite2_pos_x,sprite2_pos_y);
	//		IwGxPrintString(2, 80, string);
	//	}
	//}
	//{
	//	int16 x, y;
	//	CIwVec3 worldpos(0, 0, 0);
	//	IwGxWorldToScreenXY(x, y, worldpos);
	//	x = IwGxGetScreenWidth() - x; 
	//	y = IwGxGetScreenHeight() - y; 
	//	sprintf(string, "`1`a  p%03d, %03d", x, y);
	//	IwGxPrintString(2, 86, string);
	//}
}
