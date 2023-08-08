#pragma once

struct PacketHeader
{
	int Size;
	int Code;
};

const int CODE_InitializeGame = 1;
struct InitializeGame
{
	PacketHeader Header;
	int Seeder;
};