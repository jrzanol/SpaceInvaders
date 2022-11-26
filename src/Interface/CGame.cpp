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

    // Load Stars.
    for (int i = 0; i < 100; ++i)
        CWindow::CreateModel(5, "Mesh/Star.obj");

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
            float movPerFrame = (10.f * g_DeltaTime);
            glm::vec3* bulletpos = CheckBulletInMyWay(pos);

            if (it->m_DecisionTimer <= g_LastTime)
                it->m_DecisionTimer = 0;

            if (pos->x > -8.f && pos->x < 8.f && ((it->m_DecisionTimer == 0 || it->m_DecisionTimer > g_LastTime) && bulletpos != NULL))
            {
                if (it->m_DecisionTimer == 0)
                {
                    it->m_DecisionTimer = g_LastTime + 6.f;

                    if (bulletpos->x > pos->x)
                        it->m_DecisionOp = true;
                    else
                        it->m_DecisionOp = false;
                }

                if (it->m_DecisionOp)
                    movPerFrame = -movPerFrame;

                if ((g_LastTime - it->m_DecisionTimer) < 3.f)
                {
                    newPos.x = (pos->x + movPerFrame);

                    if (newPos.x < -8.f)
                        newPos.x = -8.f;
                    else if (newPos.x > 8.f)
                        newPos.x = 8.f;
                }
            }
            else if (newPos.z < -10.f)
            {
                newPos.z = (pos->z + movPerFrame);

                if (newPos.z > -10.f)
                    newPos.z = -10.f;
            }

            if (*pos != newPos && CheckMovement(newPos, it) == NULL)
                *pos = newPos;
        }
        else if (it->m_ModelType == 4)
        { // Tiros.
            if (newPos.z > -28.f)
            {
                const float diff = (g_LastTime - it->m_SpawnTime);
                if (diff > 0.f)
                {
                    pos->z = (it->m_InitPosition.z - (50.f * diff));

                    CModel* enemy = CheckBulletAttack(*pos);
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

glm::vec3* CGame::CheckBulletInMyWay(const glm::vec3* pos)
{
    for (int Id = 1; Id < MAX_OBJECT; ++Id)
    {
        CModel* it = CModel::GetModel(Id);

        if (!it || it->m_ModelType != 4)
            continue; // Bullet only.

        glm::vec3* enemypos = it->GetPosition();

        if ((abs(enemypos->z) - abs(pos->z)) >= 0.5f)
            continue; // Vision Field.

        float minx = (enemypos->x - 1.0f);
        float maxx = (enemypos->x + 1.0f);

        if (pos->x > minx && pos->x < maxx)
            return enemypos;
    }

    return NULL;
}

CModel* CGame::CheckMovement(glm::vec3 newPosition, CModel* thisModel)
{
    for (int Id = 1; Id < MAX_OBJECT; ++Id)
    {
        CModel* it = CModel::GetModel(Id);

        if (!it || it == thisModel || it->m_ModelType == 0 || it->m_ModelType >= 4)
            continue;

        glm::vec3* pos = it->GetPosition();

        float minx = (pos->x - 1.5f);
        float maxx = (pos->x + 1.5f);
        float minz = (pos->z - 2.2f);
        float maxz = (pos->z + 2.2f);

        if (newPosition.x > minx && newPosition.x < maxx && newPosition.z >= minz && newPosition.z <= maxz)
            return it;
    }

    return NULL;
}

CModel* CGame::CheckBulletAttack(glm::vec3 newPosition, CModel* thisModel)
{
    for (int Id = 1; Id < MAX_OBJECT; ++Id)
    {
        CModel* it = CModel::GetModel(Id);

        if (!it || it == thisModel || it->m_ModelType == 0 || it->m_ModelType >= 4)
            continue;

        glm::vec3* pos = it->GetPosition();

        float minx = (pos->x - 1.f);
        float maxx = (pos->x + 1.f);
        float minz = (pos->z - 2.f);
        float maxz = (pos->z + 2.f);

        if (newPosition.x > minx && newPosition.x < maxx && newPosition.z >= minz && newPosition.z <= maxz)
            return it;
    }

    return NULL;
}

