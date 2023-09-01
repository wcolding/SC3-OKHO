#pragma once
#include "LiteSDK.h"

class PawnHealth
{
public:
	int current = 0;
	int max = 0;

	bool IsHealthFull();
	bool IsPawnDead();
};

class SC3Core
{
private:
	double lastIGT = 0;
public:
	LiteSDK sdk;
	bool IsIGTRunning();
	PawnHealth GetPawnHealth(LiteSDK::GameObject& pawn);
	void TryKillSam();
};