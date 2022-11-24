// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

//static const int g_StarsPosition[][2] = {
//
//};

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

    // Pre-Load Enemy Models.
    CModel::LoadModel("Mesh/Enemy.obj", false);
    CModel::LoadModel("Mesh/Enemy2.obj", false);
    CModel::LoadModel("Mesh/Enemy3.obj", false);
}

void CGame::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    CModel* pl = CModel::GetModel(0);
    if (pl)
    {
        glm::vec3 newPos = *pl->GetPosition();

        float velocity = (7.5f * g_DeltaTime);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            newPos.z -= velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            newPos.z += velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            newPos.x -= velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            newPos.x += velocity;

        if (newPos.x < -8.f)
            newPos.x = -8.f;

        if (newPos.x > 8.f)
            newPos.x = 8.f;

        if (newPos.z < -5.f)
            newPos.z = -5.f;

        if (newPos.z > 0.f)
            newPos.z = 0.f;

        *pl->GetPosition() = newPos;
    }
}

void CGame::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            CWindow::CreateModel(4, "Mesh/Bullet.obj");
    }
}

void CGame::ProcessMiliSecTimer()
{
    for (int Id = 1; Id < MAX_OBJECT; ++Id)
    {
        CModel* it = CModel::GetModel(Id);
        if (!it)
            continue;

        glm::vec3* pos = it->GetPosition();
        glm::vec3 newPos = *pos;

        if (it->m_ModelType >= 1 && it->m_ModelType <= 3)
        { // Inimigos.
            if (newPos.z < -10.f && !it->m_StopMovement)
            {
                const float diff = (g_LastTime - it->m_SpawnTime);
                if (diff > 0.f)
                {
                    newPos.z = (it->m_InitPosition.z + (10.f * diff));

                    if (newPos.z > -10.f)
                        newPos.z = -10.f;

                    if (CheckMovement(newPos, it) == NULL)
                        *pos = newPos;
                    else
                        it->m_StopMovement = true;
                }
            }

            if (newPos.z < -28.f && it->m_StopMovement)
                it->m_StopMovement = false; // Ativa novamente na parte que não é viísvel.
        }
        else if (it->m_ModelType == 4)
        { // Tiros.
            if (newPos.z > -28.f)
            {
                const float diff = (g_LastTime - it->m_SpawnTime);
                if (diff > 0.f)
                {
                    pos->z = (it->m_InitPosition.z - (20.f * diff));

                    CModel* enemy = CheckMovement(*pos);
                    if (enemy != NULL)
                    {
                        CModel::DeleteModel(it);
                        CModel::DeleteModel(enemy);
                    }
                }
            }
            else
                CModel::DeleteModel(it);
        }
    }
}

// Tem que limitar o nascimento de inimigos.
void CGame::ProcessSecTimer()
{
    int _rand = (rand() % 3);
    if (_rand == 0)
        CWindow::CreateModel(1, "Mesh/Enemy.obj");
    else if (_rand == 1)
        CWindow::CreateModel(2, "Mesh/Enemy2.obj");
    else if (_rand == 2)
        CWindow::CreateModel(3, "Mesh/Enemy3.obj");
}

CModel* CGame::CheckMovement(glm::vec3 newPosition, CModel* thisModel)
{
    for (int Id = 1; Id < MAX_OBJECT; ++Id)
    {
        CModel* it = CModel::GetModel(Id);

        if (!it || it == thisModel || it->m_ModelType == 0 || it->m_ModelType >= 4)
            continue;

        glm::vec3* pos = it->GetPosition();

        float minx = (pos->x - 1.0f);
        float maxx = (pos->x + 1.0f);
        float minz = (pos->z - 2.f);
        float maxz = (pos->z + 2.f);

        if (newPosition.x > minx && newPosition.x < maxx && newPosition.z >= minz && newPosition.z <= maxz)
            return it;
    }

    return NULL;
}