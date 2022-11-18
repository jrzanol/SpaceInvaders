// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

CGame::CGame() : CEvent()
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

void CGame::ProcessMiliSecTimer()
{
    for (unsigned int Id = 0; Id < CWindow::GetModelCount(); ++Id)
    {
        CModel* it = CWindow::GetModel(Id);

        glm::vec3* pos = it->GetPosition();
        if (pos->z < -20.f)
        {
            const float diff = (g_LastTime - it->m_SpawnTime);
            if (diff > 0.f)
                pos->z = (it->m_InitPosition.z + ((10.f / 15.f) * diff));
        }
    }
}

void CGame::ProcessSecTimer()
{
    // Load Enemys.
    CModel* enemy = CWindow::CreateModel(1, "Mesh/Enemy.obj");
}

