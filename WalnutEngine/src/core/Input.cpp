#include "input.h"

#include <cstring>

Input::Input() :
	mMouseX(0),
	mMouseY(0)
	//m_window(window)
{
	memset(mInputs, 0, NUM_KEYS * sizeof(bool));
	memset(mDownKeys, 0, NUM_KEYS * sizeof(bool));
	memset(mUpKeys, 0, NUM_KEYS * sizeof(bool));

	memset(mMouseInput, 0, NUM_MOUSEBUTTONS * sizeof(bool));
	memset(mDownMouse, 0, NUM_MOUSEBUTTONS * sizeof(bool));
	memset(mUpMouse, 0, NUM_MOUSEBUTTONS * sizeof(bool));

	mMouseY = -1;
	mMouseX = -1;
}
