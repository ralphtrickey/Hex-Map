#if !defined(_CINPUT_H_)
#define _CINPUT_H_

#include "IwGeom.h"
#include "s3ePointer.h"

#define MAX_TOUCHES		S3E_POINTER_TOUCH_MAX

//
//
// CTouch - Represents a single touch
//
//
struct CTouch
{
public:
    int16		x, y;		// Touch position
    int16		x_initial, y_initial;		// Touch position of the initial finger touch
    bool	active;		// Touch active state
    int		id;			// ID of touch - The system tracks multiple touches by assigning each one a unique ID
};
//
//
// CInput - The CInput class is responsible for detecting multiple screen touches
//
//
class CInput
{
	// Properties
private:
	bool		Available;						// true if a pointer is present
	bool		IsMultiTouch;					// true if multitouch is enabled
	CTouch		Touches[MAX_TOUCHES];			// List of potential touches
public:
	bool		isAvailable() const { return Available; }			// Returns availability of the pointer
	bool		isMultiTouch() const { return IsMultiTouch; }		// Returns multitouch capability
	CTouch*		getTouchByID(int id);								// returns the touch identified by its id
	CTouch*		getTouch(int index) { return &Touches[index]; }		// Gets a specific touch
	CTouch*		findTouch(int id);									// Finds a specific touch by its id
	void	    clearAfter(int id);									// Finds a specific touch by its id
	int			getTouchCount() const;								// Get number of touches this frame
	// Properties end

private:
public:
	bool Init();							// Initialises the input system (returns true if pointer is supported)
	void Release();						// Releases data used by the input system
	void Update();						// Updates the input system, called every frame
};

class CInput2 {
private:
	bool finger1WasDown;
	bool finger1SaveWasDown;
	bool finger2WasDown;
	bool finger2SaveWasDown;
	int16 pixelLeeway;
	CInput Input;
//	int16 sprite1_pos_x_initial;
//	int16 sprite1_pos_y_initial;
public:
	CInput2();
	bool isMultiTouch() const { return Input.isMultiTouch(); }		// Returns multitouch capability
	void setPixelLeeway(int16 vPixelLeeway=10) {pixelLeeway = vPixelLeeway;};
	bool finger1Continuing();
	bool finger2Continuing();
	bool overThreshold();
	bool finger1IsDown();
	bool finger2IsDown();
	void finger1Initial(int16 &pixelsX, int16 &pixelsY);
	bool finger1MovedTo(int16 &pixelsX, int16 &pixelsY);
	bool finger1MovementDelta(int16 &dpixelsX, int16 &dpixelsY);
	void finger2Initial(int16 &pixelsX, int16 &pixelsY);
	bool finger2MovedTo(int16 &pixelsX2, int16 &pixelsY2);
//	bool twoFingerZoom(float &zoom);
//	bool twoFingerRotateDegrees(float &angle);
	bool Init(int16 vPixelLeeway=10) {setPixelLeeway(vPixelLeeway); return Input.Init();};							// Initialises the input system (returns true if pointer is supported)
	void Release() {Input.Release();};						// Releases data used by the input system
	void Update();											// Updates the input system, called every frame
};

extern CInput2 g_Input;


#endif	// _CINPUT_H_


