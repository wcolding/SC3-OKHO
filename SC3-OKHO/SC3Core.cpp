#include "SC3Core.h"

bool SC3Core::IsHealthFull(LiteSDK::GameObject& pawn)
{
    if (pawn.IsNotNull())
    {
        int curHealth = pawn.GetPropertyValue<int>("Engine.Pawn.Health");
        if (curHealth <= 0)
            return true;

        int maxHealth = pawn.GetPropertyValue<int>("Echelon.EPawn.InitialHealth");
        return curHealth == maxHealth;
    }

    return false;
}

bool SC3Core::IsIGTRunning()
{
    auto level = sdk.GetFirstInstance("Engine.Level");

    if (level.IsNotNull())
    {
        double curIGT = level.GetValueAt<double>(0xC4);
        bool timeChanged = curIGT != lastIGT;
        lastIGT = curIGT;
        return timeChanged;
    }

    return false;
}

void SC3Core::TryKillSam()
{
    auto playerController = sdk.GetFirstInstance("EPlayerController");
    auto killSam = sdk.GetBaseObject("Echelon.EPlayerController.KillSam");

    if (playerController.IsNotNull() && killSam.IsNotNull())
        playerController.ProcessEvent(killSam, nullptr, nullptr);
}