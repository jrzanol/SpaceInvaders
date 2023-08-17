#pragma once

#include <ctime>

enum class ceState
{
	Waiting = 0,
	Playing,
	Dead
};

struct PacketHeader
{
	int Size;
	int Checksum;

	int Code;
	int ID;

	ceState State;
	time_t Timestamp;
};

const int CODE_MSG_RequestInitializeGame = 1;
struct MSG_RequestInitializeGame
{
	PacketHeader Header;
};

const int CODE_MSG_InitializeGame = 2;
struct MSG_InitializeGame
{
	PacketHeader Header;
	int m_Seeder;
	int m_Count;
};

const int CODE_MSG_Action = 3;
struct MSG_Action
{
	PacketHeader Header;
	float m_DiffX;
	float m_DiffY;
	float m_DiffZ;
};

const int CODE_MSG_Dead = 4;
struct MSG_Dead
{
	PacketHeader Header;
	bool m_AllIsDead;
};

const int CODE_MSG_Attack = 5;
struct MSG_Attack
{
	PacketHeader Header;
	float m_X;
	float m_Y;
	float m_Z;
};

