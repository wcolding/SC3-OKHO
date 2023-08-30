#pragma once
#include "LiteSDK.h"

class SC3Core
{
private:
	double lastIGT = 0;
public:
	LiteSDK sdk;
	bool IsIGTRunning();
	bool IsHealthFull(LiteSDK::GameObject& pawn, int& curHealth);
	void TryKillSam();
};