/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011-2012 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License version 3 as
*    published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "miscCharacter.h"

#include "config.h"
#include "defines.h"

ConflictSide Misc::Character::GetSide(const uint8 & race)
{
    switch (race)
    {
        case RACE_BLOOD_ELF:
        case RACE_ORC:
        case RACE_TAUREN:
        case RACE_TROLL:
        case RACE_UNDEAD:
            return SIDE_HORDE;
        case RACE_DRAENEI:
        case RACE_DWARF:
        case RACE_GNOME:
        case RACE_HUMAN:
        case RACE_NIGHT_ELF:
            return SIDE_ALLIANCE;
        default:
            return SIDE_UNKNOWN;
    }
}

bool Misc::Character::SameSide(const uint8 & race1, const uint8 & race2)
{
    return GetSide(race1) == GetSide(race2);
}

uint32 Misc::Character::CalculateTalentCost(uint32 lastCost, uint32 months)
{
    // The first time reset costs 1 gold
    if (lastCost < 1*GOLD)
        return 1*GOLD;
    // then 5 gold
    else if (lastCost < 5*GOLD)
        return 5*GOLD;
    // After that it increases in increments of 5 gold
    else if (lastCost < 10*GOLD)
        return 10*GOLD;
    else
    {
        if (months > 0)
        {
            // This cost will be reduced by a rate of 5 gold per month
            uint64 reduction = 5*GOLD*months;
            if (reduction > lastCost)
                reduction = lastCost;

            int64 new_cost = lastCost - reduction;
            // to a minimum of 10 gold.
            return (new_cost < 10*GOLD ? 10*GOLD : new_cost);
        }
        else
        {
            // After that it increases in increments of 5 gold
            uint32 new_cost = lastCost + 5*GOLD;
            // until it hits a cap of 50 gold.
            if (new_cost > 50*GOLD)
                new_cost = 50*GOLD;
            return new_cost;
        }
    }
}

void Misc::Character::GetTeleportPosition(int race, Location & loc)
{
    if (race < 0 || race > 11 || race == 9)
        return;

    loc = sConfig.GetStartingLocation(CharacterRaces(race));
}

std::string Misc::Character::GetGlobalRaceName(int index)
{
    switch (index)
    {
        case RACE_HUMAN:
            return "Human";
        case RACE_ORC:
            return "Orc";
        case RACE_DWARF:
            return "Dwarf";
        case RACE_NIGHT_ELF:
            return "NightElf";
        case RACE_UNDEAD:
            return "Undead";
        case RACE_TAUREN:
            return "Tauren";
        case RACE_GNOME:
            return "Gnome";
        case RACE_TROLL:
            return "Troll";
        case RACE_BLOOD_ELF:
            return "BloodElf";
        case RACE_DRAENEI:
            return "Draenei";
        default:
            return "Unknown";
    }
}

Wt::WString Misc::Character::GetRaceName(int index)
{
    switch (index)
    {
        case RACE_HUMAN:
            return Wt::WString::tr(TXT_RACE_HUMAN);
        case RACE_ORC:
            return Wt::WString::tr(TXT_RACE_ORC);
        case RACE_DWARF:
            return Wt::WString::tr(TXT_RACE_DWARF);
        case RACE_NIGHT_ELF:
            return Wt::WString::tr(TXT_RACE_NIGHT_ELF);
        case RACE_UNDEAD:
            return Wt::WString::tr(TXT_RACE_UNDEAD);
        case RACE_TAUREN:
            return Wt::WString::tr(TXT_RACE_TAUREN);
        case RACE_GNOME:
            return Wt::WString::tr(TXT_RACE_GNOME);
        case RACE_TROLL:
            return Wt::WString::tr(TXT_RACE_TROLL);
        case RACE_BLOOD_ELF:
            return Wt::WString::tr(TXT_RACE_BLOOD_ELF);
        case RACE_DRAENEI:
            return Wt::WString::tr(TXT_RACE_DRAENEI);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
    }
}

Wt::WString Misc::Character::GetClassName(int index)
{
    switch (index)
    {
        case CLASS_WARRIOR:
            return Wt::WString::tr(TXT_CLASS_WARRIOR);
        case CLASS_PALADIN:
            return Wt::WString::tr(TXT_CLASS_PALADIN);
        case CLASS_HUNTER:
            return Wt::WString::tr(TXT_CLASS_HUNTER);
        case CLASS_ROGUE:
            return Wt::WString::tr(TXT_CLASS_ROGUE);
        case CLASS_PRIEST:
            return Wt::WString::tr(TXT_CLASS_PRIEST);
        case CLASS_SHAMAN:
            return Wt::WString::tr(TXT_CLASS_SHAMAN);
        case CLASS_MAGE:
            return Wt::WString::tr(TXT_CLASS_MAGE);
        case CLASS_WARLOCK:
            return Wt::WString::tr(TXT_CLASS_WARLOCK);
        case CLASS_DRUID:
            return Wt::WString::tr(TXT_CLASS_DRUID);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
    }
}

Wt::WString Misc::Character::GetQuestStatus(int index, bool rewarded)
{
    if (rewarded)
        return Wt::WString::tr(TXT_QUEST_STATUS_REWARDED);

    switch (index)
    {
        case 0:
            return Wt::WString::tr(TXT_QUEST_STATUS_NONE);
        case 1:
            return Wt::WString::tr(TXT_QUEST_STATUS_COMPLETE);
        case 2:
            return Wt::WString::tr(TXT_QUEST_STATUS_UNAVAILABLE);
        case 3:
            return Wt::WString::tr(TXT_QUEST_STATUS_INCOMPLETE);
        case 4:
            return Wt::WString::tr(TXT_QUEST_STATUS_AVAILABLE);
        default:
            return Wt::WString::tr(TXT_GEN_UNKNOWN);
    }
}
