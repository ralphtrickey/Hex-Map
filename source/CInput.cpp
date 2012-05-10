#include "CInput.h"

//
// Global declaration of the CInput class. Gives callback handlers access to the instantiated CInput object
// I actually turn this class into a singleton, but thats a topic for another blog
//
CInput2 g_Input;
// Used by the callbacks. I'm not sure what the normal patter for this might be. I could use friends, but that seems to expose more of the internals.
static CInput *inputPrivate;
// 
//
// Input callback handlers
//
//
//
// HandleMultiTouchButtonCB - For multitouch devices the system will call this callback when the user touches the screen. This callback is called once for each screen touch
// 
void HandleMultiTouchButtonCB(s3ePointerTouchEvent* event)
{
	// Check to see if the touch already exists
	CTouch* touch = inputPrivate->findTouch(event->m_TouchID);
    if (touch != NULL)
    {
		// Yes it does, so update the touch information
        touch->active = event->m_Pressed != 0; 
        touch->x = event->m_x;
        touch->y = event->m_y;
		if (!touch->active)
			inputPrivate->clearAfter(event->m_TouchID);
    }
}
//
// HandleMultiTouchMotionCB - For multitouch devices the system will call this callback when the user moves their finger on the screen. This callback is called once for each screen touch
// 
void HandleMultiTouchMotionCB(s3ePointerTouchMotionEvent* event)
{
	// Check to see if the touch already exists
	CTouch* touch = inputPrivate->findTouch(event->m_TouchID);
    if (touch != NULL)
    {
		// Updates the touches positional information
        touch->x = event->m_x;
        touch->y = event->m_y;
    }
}
//
// HandleSingleTouchButtonCB - The system will call this callback when the user touches the screen
// 
void HandleSingleTouchButtonCB(s3ePointerEvent* event)
{
	CTouch* touch = inputPrivate->getTouch(0);
    touch->active = event->m_Pressed != 0;
    touch->x = event->m_x;
    touch->y = event->m_y;
}
//
// HandleSingleTouchMotionCB - The system will call this callback when the user moves their finger on the screen
// 
void HandleSingleTouchMotionCB(s3ePointerMotionEvent* event)
{
	CTouch* touch = inputPrivate->getTouch(0);
    touch->x = event->m_x;
    touch->y = event->m_y;
}

//
//
// CInput implementation
//
//
CTouch*	CInput::findTouch(int id)
{
	if (!Available)
		return NULL;

	// Attempt to find the touch by its ID and then return it
	// If the touch does not exist then it is recorded in the touches list
	for (int t = 0; t < MAX_TOUCHES; t++)
	{
		if (Touches[t].id == id) {
			return &Touches[t];
		}
		if (!Touches[t].active) {
            Touches[t].id = id;
			return &Touches[t];
		}
	}

	return NULL;
}

void	CInput::clearAfter(int id)
{
	if (!Available)
		return;
	bool clearing = false;

	// Attempt to find the touch by its ID and then return it
	// If the touch does not exist then it is recorded in the touches list
	for (int t = 0; t < MAX_TOUCHES; t++)
	{
		if (Touches[t].id == id)
			clearing = true;
		if (clearing)
			Touches[t].active = false;
	}

	return;
}

CTouch*	CInput::getTouchByID(int id)
{
	// Find touch by its ID and return it
	for (int t = 0; t < MAX_TOUCHES; t++)
	{
		if (Touches[t].active && Touches[t].id == id)
			return &Touches[t];
	}

	return NULL;
}

int CInput::getTouchCount() const
{
	if (!Available)
		return 0;

	// Return the total number of active touches
	int count = 0;
	for (int t = 0; t < MAX_TOUCHES; t++) {
		if (Touches[t].active)
            count++;
		else
			break;
	}

	return count;
}

bool CInput::Init()
{
	// Check to see if the device that we are running on supports the pointer
    Available = s3ePointerGetInt(S3E_POINTER_AVAILABLE) ? true : false;
	if (!Available)
		return false;	// No pointer support

	// Clear out the touches array
	for (int t = 0; t < MAX_TOUCHES; t++)
	{
		Touches[t].active = false;
		Touches[t].id = 0;
	}

	// Determine if the device supports multi-touch
    IsMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? true : false;

	// For multi-touch devices we handle touch and motion events using different callbacks
    if (IsMultiTouch)
    {
        s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB, NULL);
        s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB, NULL);
    }
    else
    {
        s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB, NULL);
        s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB, NULL);
    }

	return true; // Pointer support
}

void CInput::Release()
{
	if (Available)
	{
		// Unregister the pointer system callbacks
		if (IsMultiTouch)
		{
			s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB);
			s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB);
		}
		else
		{
			s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB);
			s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB);
		}
	}
}

// Updates the input system, called every frame
void CInput::Update()
{
	// Update the pointer if it is available
	if (Available)
		s3ePointerUpdate();
}

CInput2::CInput2() : finger1Down(false), finger1SaveWasDown(false),
					 finger2Down(false), finger2SaveWasDown(false),
					 finger2WasDownAndFinger1IsDown(false) {inputPrivate = &Input;};

void CInput2::resetInitial(int id) {
	CTouch* touch = Input.getTouchByID(id);
	if (touch != NULL) {
		touch->x_initial = touch->x;
		touch->y_initial = touch->y;
	}
};

void CInput2::Update() {
	Input.Update();
	int touchCount = Input.getTouchCount();
	if (touchCount == 0) {
		if (finger2WasDownAndFinger1IsDown)
			finger2WasDownAndFinger1IsDown = false;
		if (finger1Down)
			finger1Down = false;
		if (finger2Down)
			finger2Down = false;
		return;
	}

	if (!finger1Down) {
		resetInitial(0);
	}
	finger1SaveWasDown = finger1Down;
	finger1Down = touchCount > 0;

	if (finger2Down) {
//		resetInitial(0);
	} else {
		resetInitial(1);
	}
	finger2SaveWasDown = finger2Down;
	finger2Down = touchCount > 1;
	if (finger1Down && !finger2Down && finger2SaveWasDown)
		finger2WasDownAndFinger1IsDown = true;
}

bool CInput2::finger1IsDown() {
	return finger1Down && !finger2WasDownAndFinger1IsDown;
}
bool CInput2::finger2IsDown() {
	return finger2Down;
}
bool CInput2::finger1Continuing() {
	return finger1SaveWasDown;
}
bool CInput2::finger2Continuing() {
	return finger2SaveWasDown;
}
bool CInput2::overThreshold() {
	return true;
	//CTouch* touch = Input.getTouchByID(0);
	//if (touch != NULL) {
	//	int dx = touch->x_initial-touch->x;
	//	int dy = touch->y_initial-touch->y;
	//	return (abs(dx) + abs(dy) > pixelLeeway);
	//}
	//return false;
}
void CInput2::finger1Initial(int16 &pixelsX, int16 &pixelsY) {
	CTouch* touch = Input.getTouchByID(0);
	if (touch != NULL) {
		pixelsX = touch->x_initial;
		pixelsY = touch->y_initial;
	}
}
bool CInput2::finger1MovedTo(int16 &pixelsX, int16 &pixelsY) {
	CTouch* touch = Input.getTouchByID(0);
	if (touch != NULL) {
		pixelsX = touch->x;
		pixelsY = touch->y;
		return true;
	}
	return false;
}
void CInput2::finger2Initial(int16 &pixelsX, int16 &pixelsY) {
	CTouch* touch = Input.getTouchByID(1);
	if (touch != NULL) {
		pixelsX = touch->x_initial;
		pixelsY = touch->y_initial;
	}
}
bool CInput2::finger2MovedTo(int16 &pixelsX2, int16 &pixelsY2) {
	if (Input.getTouchCount()<2)
		return false;
	CTouch* touch2 = Input.getTouchByID(1);
	if (touch2 == NULL)
		return false;
	else {
		pixelsX2 = touch2->x;
		pixelsY2 = touch2->y;
	}
	return true;
}
bool CInput2::finger1MovementDelta(int16 &dpixelsX, int16 &dpixelsY) {
	int16 pixelsX1=0, pixelsY1=0, pixelsX2, pixelsY2;
	if (finger1MovedTo(pixelsX2, pixelsY2)) {
		finger1Initial(pixelsX1, pixelsY1);
		dpixelsX = pixelsX2 - pixelsX1;
		dpixelsY = pixelsY2 - pixelsY1;
		return true;
	} else {
		return false;
	}
}
