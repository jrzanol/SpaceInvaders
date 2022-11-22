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
        glm::vec3 newPos = *pos;

        if (newPos.z < -10.f && !it->m_StopMovement)
        {
            const float diff = (g_LastTime - it->m_SpawnTime);
            if (diff > 0.f)
            {
                newPos.z = (it->m_InitPosition.z + ((100.f / 15.f) * diff));

                if (newPos.z > -10.f)
                    newPos.z = -10.f;

                if (CheckMovement(it, newPos))
                    *pos = newPos;
                else
                    it->m_StopMovement = true;
            }
        }

        if (newPos.z < -20.f && it->m_StopMovement)
            it->m_StopMovement = false; // Ativa novamente na parte que não é viísvel.
    }
}

void CGame::ProcessSecTimer()
{
    static int counter = 0;

    //if (counter++ == 0)
    {
        // Load Enemys.
        CModel* enemy = CWindow::CreateModel(1, "Mesh/Enemy.obj");
    }
}

bool CGame::CheckMovement(CModel* thisModel, glm::vec3 newPosition)
{
    glm::vec3 shipSize = glm::vec3(1.5f, 1.5f, 0.f);

    for (unsigned int Id = 0; Id < CWindow::GetModelCount(); ++Id)
    {
        CModel* it = CWindow::GetModel(Id);

        if (it == thisModel)
            continue;

        glm::vec3* pos = it->GetPosition();

        float minx = (pos->x - 1.5f);
        float maxx = (pos->x + 1.5f);
        float minz = (pos->z - 2.f);
        float maxz = (pos->z + 2.f);

        if (newPosition.x > minx && newPosition.x < maxx && newPosition.z >= minz && newPosition.z <= maxz)
            return false;
    }

    return true;
}