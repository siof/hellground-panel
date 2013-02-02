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

#ifndef MISCCHARACTER_H_INCLUDED
#define MISCCHARACTER_H_INCLUDED

#include "defines.h"

namespace Misc
{
    namespace Character
    {
        /********************************************//**
         * \brief Return default hardcoded character race name.
         *
         * \param sess      session informations to get text
         * \param index     race index
         * \return race name
         *
         * Returns default hardcoded character race name depends on race index.
         *
         ***********************************************/

        std::string GetGlobalRaceName(int index);

        /********************************************//**
         * \brief Return character race name.
         *
         * \param sess      session informations to get text
         * \param index     race index
         * \return race name
         *
         * Returns character race name depends on race index.
         *
         ***********************************************/

        Wt::WString GetRaceName(int index);

        /********************************************//**
         * \brief Return character class name.
         *
         * \param sess      session informations to get text
         * \param index     class index
         * \return class name
         *
         * Returns character class name depends on class index.
         *
         ***********************************************/

        Wt::WString GetClassName(int index);

        /********************************************//**
         * \brief Return character quest status in string.
         *
         * \param sess      session informations to get text
         * \param index     quest status index
         * \param rewarded  information that quest is already rewarded or not
         * \return quest status
         *
         * Returns character quest status in string depends on status index and rewarded info.
         *
         ***********************************************/

        Wt::WString GetQuestStatus(int index, bool rewarded);

        /********************************************//**
         * \brief Returns conflict side.
         *
         * \param race  character race
         * \return inforamtion about side in conflict.
         *
         ***********************************************/

        ConflictSide GetSide(const uint8 & race);

        /********************************************//**
         * \brief Returns informations if races are in same faction
         *
         * \param race1 first character race
         * \param race2 second character race
         * \return information that races are in same faction or not
         *
         * Function checks if given races are in same side and returns this information.
         *
         ***********************************************/

        bool SameSide(const uint8 & race1, const uint8 & race2);

        /********************************************//**
         * \brief Calculates and returns reset talent cost.
         *
         * \param lastCost  last reset talent cost
         * \param monts     count of months from last reset talent time
         * \return actual reset talent cost
         *
         ***********************************************/

        uint32 CalculateTalentCost(uint32 lastCost, uint32 months);

        /********************************************//**
         * \brief Sets teleport position depends on race.
         *
         * \param race  Character race.
         * \param loc   Variable in which location will be stored.
         *
         ***********************************************/

        void GetTeleportPosition(int race, Location & loc);
    }
}

#endif // MISCCHARACTER_H_INCLUDED
