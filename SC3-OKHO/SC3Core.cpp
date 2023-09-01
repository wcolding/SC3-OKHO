#include "SC3Core.h"

bool SC3Core::IsHealthFull(LiteSDK::GameObject& pawn, int& curHealth)
{
    if (pawn.IsNotNull())
    {
        curHealth = pawn.GetPropertyValue<int>("Engine.Pawn.Health");
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

    if (sdk.ValidateObjects())
        playerController.ProcessEvent(killSam, nullptr, nullptr);
}