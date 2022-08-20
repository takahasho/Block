#pragma once
#include "MoveComponent.h"
#include <atlbase.h>            // ATL 

class PlayerInputComponent :
	public MoveComponent
{
public:
	PlayerInputComponent(class Actor* owner);
	void ProcessInput() override;
	static bool GetShowCursor() { return mShowCursor; }
	void SetShowCursor();
private:
	BOOL mPushMKey = false;
	static BOOL mShowCursor;
	BOOL mCursor = false;
};