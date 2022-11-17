// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

CGame::CGame()
{
}

CGame::~CGame()
{
}

void CGame::Initialize()
{
    // Load Models.
    CModel* player = CWindow::CreateModel(0, "Mesh/Player.obj");
}

