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

#include <ctime>

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief Slots for Basic Character Informations
 *
 * Must be same as order on page.
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
 * Must be same as order on page.
 ***********************************************/

enum CharacterQuestInfoSlot
{
    CHARQUESTINFO_SLOT_NAME     = 0,        /**< Quest name label. */
    CHARQUESTINFO_SLOT_LEVEL    = 1,        /**< Quest level label. */
    CHARQUESTINFO_SLOT_STATUS   = 2,        /**< Quest status label. */

    CHARQUESTINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Spells Informations
 *
 * Must be same as order on page.
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
 * Must be same as order on page.
 ***********************************************/

enum CharacterInventoryInfoSlot
{
    CHARINVINFO_SLOT_ID     = 0,            /**< Item ID label. */
    CHARINVINFO_SLOT_NAME   = 1,            /**< Item name label. */
    CHARINVINFO_SLOT_STACK  = 2,            /**< Items count in stack. */

    CHARINVINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Friend Informations
 *
 * Must be same as order on page.
 ***********************************************/

enum CharacterFriendInfoSlot
{
    CHARFRIENDINFO_SLOT_NAME    = 0,        /**< Friend name label. */
    CHARFRIENDINFO_SLOT_NOTE    = 1,        /**< Friend note label. */
    CHARFRIENDINFO_SLOT_ONLINE  = 2,        /**< Friend is online/offline label. */

    CHARFRIENDINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for Character Friend Informations
 *
 * Must be same as order on page.
 ***********************************************/

enum CharacterMailsSlot
{
    CHARMAILS_SLOT_FROM     = 0,            /**< From who is this mail. */
    CHARMAILS_SLOT_TITLE    = 1,            /**< Mail title. */
    CHARMAILS_SLOT_DATE     = 2,            /**< Delivery date. */
    CHARMAILS_SLOT_EXPIRES  = 3,            /**< Expire date. */
    CHARMAILS_SLOT_READED   = 4,            /**< Infomations if mail was readed. */

    CHARMAILS_SLOT_COUNT
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
    CHAR_TAB_FRIENDS        = 4,            /**< Friends informations tab */
    CHAR_TAB_MAILS          = 5,            /**< Mail informations tab */

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
 * \brief Structure to store character mail informations
 *
 * This structure is used to store some informations
 * needed by mail preview system.
 *
 ***********************************************/

/*Table: mail
Columns:
id	int(11) UN PK
messageType	tinyint(3) UN
stationery	tinyint(3)
mailTemplateId	mediumint(8) UN
sender	int(11) UN
receiver	int(11) UN
subject	longtext
itemTextId	int(11) UN
has_items	tinyint(3) UN
expire_time	bigint(40)
deliver_time	bigint(40)
money	int(11) UN
cod	int(11) UN
checked	tinyint(3) UN */

struct DatabaseRow;

struct MailInfo
{
    MailInfo() {}
    MailInfo(const MailInfo & mi);
    MailInfo(const DatabaseRow * row);

    ~MailInfo() {}

    void LoadItems(std::list<DatabaseRow*> & mailItems);

    Wt::WString GetFrom() const;

    Wt::WString GetSubject() const { return subject; }
    Wt::WString GetBody() const { return text; }
    Wt::WString GetDeliverTime() const { return deliverTime; }
    Wt::WString GetExpireTime() const { return expireTime; }

    bool IsReaded() const { return checkMask & MAIL_CHECK_MASK_READ; }
    bool IsCOD() const { return checkMask & MAIL_CHECK_MASK_COD_PAYMENT; }
    bool HasMoney() const { return money != 0; }
    bool HasItems() const { return !items.empty(); }

    uint64 id;
    MailMessageType type;
    MailStationery stationery;
    uint32 templateId;
    uint32 money;
    uint32 cod;
    uint64 checkMask;

    Wt::WString from;
    Wt::WString subject;
    Wt::WString text;
    Wt::WString deliverTime;
    Wt::WString expireTime;

    std::list<Item> items;
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

    static std::map<uint32, SpellInfo> spells;
    static bool spellsLoaded;

private:
    /// panel session informations
    SessionInfo * session;
    /// label with character page additional info
    WText * charPageInfo;
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
    /// last character info update time
    std::time_t lastUpdateTime;
    /// table with character mail list
    Wt::WTable * mailList;
    /// container with mail preview
    Wt::WContainerWidget * mailPreviewCont;
    /// mail preview data
    Wt::WText * mailPreviewFrom;
    Wt::WText * mailPreviewExpire;
    Wt::WText * mailPreviewSubject;
    Wt::WText * mailPreviewBody;


    std::vector<MailInfo> characterMails;

    /// informs that RestoreCharacter function is actually executed
    bool restoring;

    bool IsDeletedCharacter(const uint64 & guid);
    bool IsDeletedCharacter(const CharInfo & charInfo) { return charInfo.deleted; }

    void UpdateInformations(uint64 guid, bool force = false);

    Wt::WTable * charBasicInfo;
    Wt::WContainerWidget * CreateCharacterBasicInfo();
    void UpdateCharacterBasicInfo(uint64 guid);

    Wt::WTable * CreateCharacterQuestInfo();
    void UpdateCharacterQuestInfo(uint64 guid);

    WTable * CreateCharacterSpellInfo();
    void UpdateCharacterSpellInfo(uint64 guid);

    WTable * CreateCharacterInventoryInfo();
    void UpdateCharacterInventoryInfo(uint64 guid);

    WTable * CreateCharacterFriendInfo();
    void UpdateCharacterFriendInfo(uint64 guid);
    void ClearFriendsTable();

    Wt::WContainerWidget * CreateCharacterMailInfo();
    void UpdateCharacterMailInfo(uint64 guid);
    void ClearMails();

    void ClearPage();

    void SelectionChanged(int selected);

    void RebuildCharList();

    void RestoreCharacter();

    void LoadSpells();

    void PreviewMail(int mailIdx);
    void BindPreviewMail(Wt::EventSignal<Wt::WMouseEvent>& signal, int mailIdx);
};

#endif // CHARACTERS_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
