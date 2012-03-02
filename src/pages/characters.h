/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
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

/********************************************//**
 * \addtogroup Pages
 * \{
 *
 * \addtogroup Characters Character informations
 * Character informations contains code to represent player
 * characters information page.
 * Character informations consists of mostly 2 files:
 * - characters.h
 * - characters.cpp
 * \{
 *
 * \file characters.h
 * This file contains th headers needed to show player characters informations.
 *
 ***********************************************/


#ifndef CHARACTERS_H_INCLUDED
#define CHARACTERS_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"

#include <WTabWidget>
#include <WTable>
#include <WComboBox>

/********************************************//**
 * \brief Slots for Basic page informations.
 *
 * Also determines order on page.
 *
 ***********************************************/

enum CharInfoSlot
{
    CHARINFO_SLOT_INFO          = 0,    /**< Page title text. */
    CHARINFO_SLOT_ADDINFO,              /**< Additional info slot */
    CHARINFO_SLOT_CHARACTERS,           /**< Slot for character selection */

    CHARINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Basic Character Informations
 *
 * Also determines order on page.
 *
 ***********************************************/

enum CharacterBasicInfoSlot
{
    CHARBASICINFO_SLOT_LEVEL                = 0,        /**< Text with character level. */
    CHARBASICINFO_SLOT_RACE                 = 1,        /**< Text with character race. */
    CHARBASICINFO_SLOT_CLASS                = 2,        /**< Text with character class. */
    CHARBASICINFO_SLOT_NAME                 = 3,        /**< Text with character name. */
    CHARBASICINFO_SLOT_ONLINE               = 4,        /**< Text with character online status. */
    CHARBASICINFO_SLOT_PLAYED_TOT           = 5,        /**< Text with character total played time. */
    CHARBASICINFO_SLOT_PLAYED_LVL           = 6,        /**< Text with character played time on current level. */
    CHARBASICINFO_SLOT_LAST_RESET_COST      = 7,        /**< Text with last character reset talent cost. */
    CHARBASICINFO_SLOT_LAST_RESET_TIME      = 8,        /**< Text with last character reset talent time. */
    CHARBASICINFO_SLOT_ACTUAL_RESET_COST    = 9,        /**< Text with character actual reset talent cost. */
    CHARBASICINFO_SLOT_DELETION_TIME        = 10,       /**< Text with time when character was deleted. */

    CHARBASICINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Quest Informations
 *
 * Also determines order on page.
 *
 ***********************************************/

enum CharacterQuestInfoSlot
{
    CHARQUESTINFO_SLOT_ID       = 0,        /**< Quest ID label. */
    CHARQUESTINFO_SLOT_NAME     = 1,        /**< Quest name label. */
    CHARQUESTINFO_SLOT_LEVEL    = 2,        /**< Quest level label. */
    CHARQUESTINFO_SLOT_STATUS   = 3,        /**< Quest status label. */

    CHARQUESTINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Spells Informations
 *
 * Also determines order on page.
 *
 ***********************************************/

enum CharacterSpellInfoSlot
{
    CHARSPELLINFO_SLOT_ID       = 0,        /**< Spell ID label. */
    CHARSPELLINFO_SLOT_NAME     = 1,        /**< Spell name label. */
    CHARSPELLINFO_SLOT_ACTIVE   = 2,        /**< Spell active info label. */
    CHARSPELLINFO_SLOT_DISABLED = 3,        /**< Spell disabled info label. */

    CHARSPELLINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Inventory Informations
 *
 * Also determines order on page.
 *
 ***********************************************/

enum CharacterInventoryInfoSlot
{
    CHARINVINFO_SLOT_ID     = 0,            /**< Item ID label. */
    CHARINVINFO_SLOT_NAME   = 1,            /**< Item name label. */
    CHARINVINFO_SLOT_STACK  = 2,            /**< Items count in stack. */

    CHARINVINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for character informations tabs
 ***********************************************/

enum CharacterTabsSlots
{
    CHAR_TAB_BASIC          = 0,            /**< Basic informations tab */
    CHAR_TAB_QUEST          = 1,            /**< Quest informations tab */
    CHAR_TAB_SPELL          = 2,            /**< Spell informations tab */
    CHAR_TAB_INVENTORY      = 3,            /**< Inventory informations tab */

    CHAR_TAB_COUNT
};

/********************************************//**
 * \brief Structure to store some character informations.
 *
 * This structure is used to store some informations
 * specially for character restore feature.
 *
 ***********************************************/

struct CharInfo
{
    CharInfo() : guid(0), account(0), name(""), race(0), deleted(false), deletionDate("") { }
    CharInfo(const uint64 & charGuid, uint32 charAcc, const WString & charName, uint8 charRace, bool del = false, const WString & delDate = ""):
        guid(charGuid), account(charAcc), name(charName), race(charRace), deleted(del), deletionDate(delDate)
    {

    }

    ~CharInfo() { }

    uint64 guid;
    uint32 account;
    WString name;
    uint8 race;
    bool deleted;
    WString deletionDate;
};

/********************************************//**
 * \brief A class to represents Character Informations page
 *
 * This class is container for other widgets with characters informations.
 * Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 *
 ***********************************************/

class CharacterInfoPage : public WContainerWidget
{
public:
    CharacterInfoPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~CharacterInfoPage();

    void refresh();
private:
    /// panel session informations
    SessionInfo * session;
    /// tab widget to group specific characters informations
    WTabWidget * tabs;
    /// tabs was created or not?
    bool needCreation;
    /// list of characters to select from
    WComboBox * charList;
    /// button to restore character
    WPushButton * restoreCharacter;
    /// combo box index to character guid map
    std::map<int, CharInfo> indexToCharInfo;

    /// informs that RestoreCharacter function is actually executed
    bool restoring;

    /// contains global page info slots like page title or additional info like errors
    PageSlotItem pageInfoSlots[CHARINFO_SLOT_COUNT];
    /// contains basic character info slots so we can update them in easy way ;)
    PageSlotItem basicInfoSlots[CHARBASICINFO_SLOT_COUNT];
    /// contains headers for quest info table
    BasicTextItem questInfoSlots[CHARQUESTINFO_SLOT_COUNT];
    /// contains headers for spell info table
    BasicTextItem spellInfoSlots[CHARSPELLINFO_SLOT_COUNT];
    /// contains headers for inventory info table
    BasicTextItem inventoryInfoSlots[CHARINVINFO_SLOT_COUNT];

    bool IsDeletedCharacter(const uint64 & guid);
    bool IsDeletedCharacter(const CharInfo & charInfo) { return charInfo.deleted; }

    void UpdateTextWidgets();
    void UpdateInformations(uint64 guid);

    WContainerWidget * CreateCharacterBasicInfo();
    void UpdateCharacterBasicInfo(uint64 guid);

    WTable * CreateCharacterQuestInfo();
    void UpdateCharacterQuestInfo(uint64 guid);

    WTable * CreateCharacterSpellInfo();
    void UpdateCharacterSpellInfo(uint64 guid);

    WTable * CreateCharacterInventoryInfo();
    void UpdateCharacterInventoryInfo(uint64 guid);

    void ClearPage(bool alsoCharList = true);

    void SelectionChanged(int selected);

    void RebuildCharList();

    void RestoreCharacter();
};

#endif // CHARACTERS_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/