// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CModel.h"
#include "CSocket.h"

class CGame : CEvent, CSocket
{
public:
	CGame();
	~CGame();

	void Initialize();

	void ProcessInput(GLFWwindow*);
	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);
	void ProcessMiliSecTimer();
	void ProcessSecTimer();
	void Process(const PacketHeader*);

	static int Rand();
	static bool m_IsDead;
	static bool m_GameOver;
	static int m_PlayerMyId;
	static int m_PlayerCount;
	static int m_PlayerCounter;

	static glm::vec3* CheckBulletInMyWay(const glm::vec3*);
	static CModel* CheckMovement(glm::vec3, CModel* = NULL);
	static CModel* CheckBulletAttack(glm::vec3, CModel* = NULL, bool = false);

private:
	int m_Points;

	static unsigned long m_Seeder;
};

