#pragma once

struct PacketHeader
{
	int Size;
	int Code;
	int ID;
};

const int CODE_MSG_InitializeGame = 1;
struct MSG_InitializeGame
{
	PacketHeader Header;
	int Seeder;
	int Count;
};

const int CODE_MSG_RequestInitializeGame = 2;
struct MSG_RequestInitializeGame
{
	PacketHeader Header;
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

