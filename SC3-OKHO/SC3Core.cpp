#include "SC3Core.h"

PawnHealth SC3Core::GetPawnHealth(LiteSDK::GameObject& pawn)
{
    PawnHealth ph;

    if (pawn.IsNotNull())
    {
        ph.current = pawn.GetPropertyValue<int>("Engine.Pawn.Health");
        ph.max = pawn.GetPropertyValue<int>("Echelon.EPawn.InitialHealth");
    }

    return ph;
}

bool PawnHealth::IsHealthFull()
{
    return current == max;
}

bool PawnHealth::IsPawnDead()
{
    return current <= 0;
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