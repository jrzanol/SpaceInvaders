// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CModel.h"

class CGame : CEvent
{
public:
	CGame();
	~CGame();

	void Initialize();

	void ProcessMiliSecTimer();
	void ProcessSecTimer();
};

