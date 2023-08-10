#pragma once

struct PacketHeader
{
	int Size;
	int Code;
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

