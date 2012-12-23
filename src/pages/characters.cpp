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
 * \{
 *
 * \file characters.cpp
 * This file contains code needed to show player characters informations.
 *
 ***********************************************/

#include "characters.h"

#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WLineF>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WTabWidget>
#include <Wt/WText>

#include "../database.h"
#include "../misc.h"
#include "../miscCharacter.h"

bool CharacterInfoPage::spellsLoaded = false;
std::map<uint32, SpellInfo> CharacterInfoPage::spells;

MailInfo::MailInfo(const MailInfo & mi)
{
    id = mi.id;
    from = mi.from;
    type = mi.type;
    stationery = mi.stationery;
    subject = mi.subject;
    deliverTime = mi.deliverTime;
    expireTime = mi.expireTime;
    text = mi.text;
    money = mi.money;
    cod = mi.cod;
    checkMask = mi.checkMask;
    templateId = mi.templateId;

    items = mi.items;
}

MailInfo::MailInfo(const DatabaseRow * row)
{
    id = row->fields[0].GetUInt64();
    from = row->fields[1].GetWString();
    type = MailMessageType(row->fields[2].GetInt());
    stationery = MailStationery(row->fields[3].GetInt());
    subject = row->fields[4].GetWString();
    deliverTime = row->fields[5].GetWString();
    expireTime = row->fields[6].GetWString();
    text = row->fields[7].GetWString();
    money = row->fields[8].GetUInt32();
    cod = row->fields[9].GetUInt32();
    checkMask = row->fields[10].GetUInt32();
}

/********************************************//**
 * \brief Loads items to mail from given database row list.
 *
 * \param mailItems list contains database result with all mailed items to given character
 *
 * Funcion iterates on given mail items list. When item attached to this mail will be found
 * function will store it's data and item will be removed from mail items list.
 ***********************************************/

void MailInfo::LoadItems(std::list<DatabaseRow*> & mailItems)
{
    // process all currently loaded mail items
    for (std::list<DatabaseRow*>::iterator itr = mailItems.begin(); itr != mailItems.end();)
    {
        std::list<DatabaseRow*>::iterator tmpItr = itr;
        ++itr;
        DatabaseRow * tmpRow = *tmpItr;

        // if it's item from this mail get store his data and remove from loaded mail items list
        if (tmpRow->fields[0].GetUInt64() == id)
        {
            Item tmpItem;
            tmpItem.id = tmpRow->fields[1].GetUInt32();
            tmpItem.name = tmpRow->fields[2].GetWString();
            tmpItem.stackCount = tmpRow->fields[3].GetUInt32();
            tmpItem.guid = tmpRow->fields[4].GetUInt32();

            items.push_back(tmpItem);

            mailItems.erase(tmpItr);
        }
    }
}

/********************************************//**
 * \brief Returns 'From' clause depends on mail stationery.
 *
 * Function returns 'From' clause depends on mail stationery.
 * Mails in game can be send by different sources (Auction House, GM, NPC, Player) and
 * we should return other values for each of them.
 ***********************************************/

Wt::WString MailInfo::GetFrom() const
{
    switch (stationery)
    {
        case MAIL_STATIONERY_GM:
            return Wt::WString::tr(TXT_MAIL_FROM_SUPPORT);
        case MAIL_STATIONERY_AUCTION:
            return Wt::WString::tr(TXT_MAIL_FROM_AUCTION);
        case MAIL_STATIONERY_CHR:
            return from;
        default:
            break;
    }

    return Wt::WString::tr(TXT_GEN_UNKNOWN);
}

/********************************************//**
 * \brief Creates new CharacterInfoPage object.
 *
 * \param sess      Contains user session informations.
 * \param parent    Parent container widget to which should be added this widget.
 *
 ***********************************************/

CharacterInfoPage::CharacterInfoPage(SessionInfo * sess, WContainerWidget * parent) :
    WContainerWidget(parent), restoreCharacter(NULL), restoring(false)
{
    session = sess;

    needCreation = true;

    setStyleClass("page characterswidget");
}

CharacterInfoPage::~CharacterInfoPage()
{
    session = NULL;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void CharacterInfoPage::refresh()
{
    // only logged in players can visit this page so there is no need to create/update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
        {
            needCreation = false;

            addWidget(new WText(Wt::WString::tr(TXT_INFO_CHARACTER)));
            addWidget(new WBreak());
            addWidget(new WBreak());

            charPageInfo = new WText("", this);
            addWidget(new WBreak());
            addWidget(new WBreak());

            addWidget(new WText(Wt::WString::tr(TXT_CHAR_LIST)));
            charList = new WComboBox(this);
            charList->activated().connect(this, &CharacterInfoPage::SelectionChanged);
            addWidget(new WBreak());
            addWidget(new WBreak());

            tabs = new WTabWidget();
            tabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);
            addWidget(tabs);

            tabs->addTab(CreateCharacterBasicInfo(), Wt::WString::tr(TXT_CHAR_TAB_INFO)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateCharacterQuestInfo(), Wt::WString::tr(TXT_CHAR_TAB_QUEST)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateCharacterSpellInfo(), Wt::WString::tr(TXT_CHAR_TAB_SPELL)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateCharacterInventoryInfo(), Wt::WString::tr(TXT_CHAR_TAB_INVENTORY)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateCharacterFriendInfo(), Wt::WString::tr(TXT_CHAR_TAB_FRIENDS)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateCharacterMailInfo(), Wt::WString::tr(TXT_CHAR_TAB_MAIL));

            charList->clear();
            indexToCharInfo.clear();
            Database db;
            if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
            {
                charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
                return;
            }

            int index = 0;
            switch (db.ExecutePQuery("SELECT guid, account, name, race FROM characters WHERE account = '%u'", session->accid))
            {
                case DB_RESULT_ERROR:
                    charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
                    return;
                case DB_RESULT_EMPTY:
                    break;
                default:

                    std::list<DatabaseRow*> rows = db.GetRows();
                    DatabaseRow * tmpRow;

                    for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
                    {
                        tmpRow = *itr;

                        CharInfo tmpCharInfo(tmpRow->fields[0].GetUInt64(), tmpRow->fields[1].GetUInt32(), tmpRow->fields[2].GetWString(), tmpRow->fields[3].GetInt(), false);

                        indexToCharInfo[index] = tmpCharInfo;
                        charList->insertItem(index++, tmpCharInfo.name);
                    }
                    break;
            }

            if (db.ExecutePQuery("SELECT char_guid, acc, oldname, race, date "
                                 "FROM deleted_chars JOIN characters ON deleted_chars.char_guid = characters.guid "
                                 "WHERE acc = '%u'", session->accid) > DB_RESULT_EMPTY)
            {

                std::list<DatabaseRow*> rows = db.GetRows();
                DatabaseRow * tmpRow;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
                {
                    tmpRow = *itr;

                    CharInfo tmpCharInfo;
                    tmpCharInfo.guid = tmpRow->fields[0].GetUInt64();
                    tmpCharInfo.account = tmpRow->fields[1].GetUInt32();
                    tmpCharInfo.name = tmpRow->fields[2].GetWString();
                    tmpCharInfo.race = tmpRow->fields[3].GetInt();
                    tmpCharInfo.deleted = true;
                    tmpCharInfo.deletionDate = tmpRow->fields[4].GetWString();

                    indexToCharInfo[index] = tmpCharInfo;
                    charList->insertItem(index++, "[Del] " + tmpCharInfo.name);
                }

                if (charList->count() > 0)
                    charList->setCurrentIndex(0);
            }

            db.Disconnect();
        }

        std::map<int, CharInfo>::const_iterator tmpItr = indexToCharInfo.find(charList->currentIndex());
        if (tmpItr != indexToCharInfo.end())
            UpdateInformations(tmpItr->second.guid);
    }
    else
        ClearPage();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update character informations.
 *
 * Function updates all informations of currenly selected character, but only
 * if update interval will be greater than configured in config file (or will be forced).
 * Minimum update interval prevents 'over updating character' if there is no need for that.
 *
 ***********************************************/

void CharacterInfoPage::UpdateInformations(uint64 guid, bool force)
{
    if (!guid)
        return;

    if (!force && lastUpdateTime + CHAR_UPDATE_INTERVAL > std::time(NULL))
        return;

    UpdateCharacterBasicInfo(guid);
    UpdateCharacterQuestInfo(guid);
    UpdateCharacterSpellInfo(guid);
    UpdateCharacterInventoryInfo(guid);
    UpdateCharacterFriendInfo(guid);

    lastUpdateTime = std::time(NULL);
}

/********************************************//**
 * \brief Create informations widgets.
 *
 * Create widgets for each slot and fills them with basic informations.
 * This should be done only once for player.
 *
 ***********************************************/

WContainerWidget * CharacterInfoPage::CreateCharacterBasicInfo()
{
    WContainerWidget * tmpContainer = new WContainerWidget();

    charBasicInfo = new Wt::WTable(tmpContainer);
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LEVEL, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_LVL)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LEVEL, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_RACE, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_RACE)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_RACE, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_CLASS, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_CLASS)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_CLASS, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_NAME, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_NAME)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_NAME, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_ONLINE, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_ONLINE)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_ONLINE, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_TOT, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_PLAYED_TOTAL)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_TOT, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_LVL, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_PLAYED_LEVEL)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_LVL, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_COST, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_TALENT_RESET_COST)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_COST, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_TIME, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_TALENT_RESET_TIME)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_TIME, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_ACTUAL_RESET_COST, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_ACT_TAL_RESET_COST)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_ACTUAL_RESET_COST, 1)->addWidget(new WText(""));

    charBasicInfo->elementAt(CHARBASICINFO_SLOT_DELETION_TIME, 0)->addWidget(new WText(Wt::WString::tr(TXT_CHAR_DELETION_DATE)));
    charBasicInfo->elementAt(CHARBASICINFO_SLOT_DELETION_TIME, 1)->addWidget(new WText(""));

    tmpContainer->addWidget(new WBreak());
    tmpContainer->addWidget(new WBreak());

    restoreCharacter = new WPushButton(tr(TXT_BTN_CHARACTER_RESTORE), tmpContainer);
    restoreCharacter->clicked().connect(this, &CharacterInfoPage::RestoreCharacter);

    return tmpContainer;
}

/********************************************//**
 * \brief Create quest informations.
 *
 * All quest completed by character and those actually in quest log
 * will be listed in this table.
 *
 ***********************************************/

WTable * CharacterInfoPage::CreateCharacterQuestInfo()
{
    WTable * tmpQuest = new WTable();

    tmpQuest->setHeaderCount(1);

    tmpQuest->elementAt(0, CHARQUESTINFO_SLOT_NAME)->addWidget(new WText(Wt::WString::tr(TXT_QUEST_NAME)));
    tmpQuest->elementAt(0, CHARQUESTINFO_SLOT_LEVEL)->addWidget(new WText(Wt::WString::tr(TXT_QUEST_LVL)));
    tmpQuest->elementAt(0, CHARQUESTINFO_SLOT_STATUS)->addWidget(new WText(Wt::WString::tr(TXT_QUEST_STATUS)));

    return tmpQuest;
}

/********************************************//**
 * \brief Create spell informations.
 *
 * All spells learned by character will be listed in this table.
 *
 ***********************************************/

WTable * CharacterInfoPage::CreateCharacterSpellInfo()
{
    LoadSpells();

    WTable * tmpSpell = new WTable();

    tmpSpell->setHeaderCount(1);

    tmpSpell->elementAt(0, CHARSPELLINFO_SLOT_ID)->addWidget(new WText(Wt::WString::tr(TXT_SPELL_ID)));
    tmpSpell->elementAt(0, CHARSPELLINFO_SLOT_NAME)->addWidget(new WText(Wt::WString::tr(TXT_SPELL_NAME)));
    tmpSpell->elementAt(0, CHARSPELLINFO_SLOT_ACTIVE)->addWidget(new WText(Wt::WString::tr(TXT_SPELL_ACTIVE)));
    tmpSpell->elementAt(0, CHARSPELLINFO_SLOT_DISABLED)->addWidget(new WText(Wt::WString::tr(TXT_SPELL_DISABLED)));

    return tmpSpell;
}

/********************************************//**
 * \brief Create inventory informations.
 *
 * All items in character inventory will be listed in this table.
 *
 ***********************************************/

WTable * CharacterInfoPage::CreateCharacterInventoryInfo()
{
    WTable * tmpInv = new WTable();

    tmpInv->setHeaderCount(1);

    tmpInv->elementAt(0, CHARINVINFO_SLOT_ID)->addWidget(new WText(Wt::WString::tr(TXT_ITEM_ID)));
    tmpInv->elementAt(0, CHARINVINFO_SLOT_NAME)->addWidget(new WText(Wt::WString::tr(TXT_ITEM_NAME)));
    tmpInv->elementAt(0, CHARINVINFO_SLOT_STACK)->addWidget(new WText(Wt::WString::tr(TXT_ITEM_COUNT)));

    return tmpInv;
}

/********************************************//**
 * \brief Create friends informations.
 *
 * All friends in character friend list will be listed in this table.
 *
 ***********************************************/

WTable * CharacterInfoPage::CreateCharacterFriendInfo()
{
    WTable * tmpFriends = new WTable();

    tmpFriends->setHeaderCount(1);

    tmpFriends->elementAt(0, CHARFRIENDINFO_SLOT_NAME)->addWidget(new WText(Wt::WString::tr(TXT_FRIEND_NAME)));
    tmpFriends->elementAt(0, CHARFRIENDINFO_SLOT_NOTE)->addWidget(new WText(Wt::WString::tr(TXT_FRIEND_NOTE)));
    tmpFriends->elementAt(0, CHARFRIENDINFO_SLOT_ONLINE)->addWidget(new WText(""));

    return tmpFriends;
}

/********************************************//**
 * \brief Create widget to contain in game mails
 *
 * All in game mails will be here.
 *
 ***********************************************/

Wt::WContainerWidget * CharacterInfoPage::CreateCharacterMailInfo()
{
    Wt::WContainerWidget * tmpCont = new Wt::WContainerWidget();
    mailList = new Wt::WTable();
    mailList->setStyleClass("maillist");

    ClearMails();

    tmpCont->addWidget(mailList);

    mailPreviewCont = new Wt::WContainerWidget();
    mailPreviewCont->setStyleClass("mailpreview");

    Wt::WBreak * tmpBreak = new Wt::WBreak();
    tmpBreak->setStyleClass("line");
    tmpCont->addWidget(tmpBreak);

    mailPreviewFrom = new Wt::WText("");
    mailPreviewFrom->setStyleClass("mailpreview mailfrom");

    mailPreviewExpire = new Wt::WText("");
    mailPreviewExpire->setStyleClass("mailpreview mailexpire");

    mailPreviewSubject = new Wt::WText("");
    mailPreviewSubject->setStyleClass("mailpreview mailsubject");

    mailPreviewBody = new Wt::WText("");
    mailPreviewBody->setStyleClass("mailpreview mailbody");

    mailPreviewCont->addWidget(mailPreviewFrom);
    mailPreviewCont->addWidget(mailPreviewExpire);
    mailPreviewCont->addWidget(new Wt::WBreak());
    mailPreviewCont->addWidget(mailPreviewSubject);
    mailPreviewCont->addWidget(new Wt::WBreak());
    mailPreviewCont->addWidget(mailPreviewBody);
    mailPreviewCont->addWidget(new Wt::WBreak());

    tmpCont->addWidget(mailPreviewCont);

    return tmpCont;
}

/********************************************//**
 * \brief Update Basic Character Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void CharacterInfoPage::UpdateCharacterBasicInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    switch (db.ExecutePQuery("SELECT level, race, class, name, online, totaltime, leveltime, resettalents_cost, FROM_UNIXTIME(resettalents_time), DATEDIFF(now(), FROM_UNIXTIME(resettalents_time)), date "
                             "FROM characters LEFT OUTER JOIN deleted_chars ON characters.guid = deleted_chars.char_guid "
                             "WHERE guid = '%u'", guid))
    {
        case DB_RESULT_ERROR:
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        case DB_RESULT_EMPTY:
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_EMPTY));
            return;
        default:
        {
            db.Disconnect();
            DatabaseRow * tmpRow = db.GetRow();

            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_LEVEL, 1)->widget(0))->setText(tmpRow->fields[0].GetWString());
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_RACE, 1)->widget(0))->setText(Misc::Character::GetRaceName(tmpRow->fields[1].GetInt()));
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_CLASS, 1)->widget(0))->setText(Misc::Character::GetClassName(tmpRow->fields[2].GetInt()));
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_NAME, 1)->widget(0))->setText(tmpRow->fields[3].GetWString());
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_ONLINE, 1)->widget(0))->setText(Wt::WString::tr(tmpRow->fields[4].GetBool() ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));

            uint64 tmpVal = tmpRow->fields[5].GetUInt64();
            int tmpDays = tmpVal/86400;

            tmpVal -= tmpDays * 86400;
            int tmpHours = tmpVal/3600;

            tmpVal -= tmpHours * 3600;
            int tmpMinutes = tmpVal/60;

            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_TOT, 1)->widget(0))->setText(Wt::WString::tr(TXT_CHAR_PLAYED_FMT).arg(tmpDays).arg(tmpHours).arg(tmpMinutes));

            tmpVal = tmpRow->fields[6].GetUInt64();
            tmpDays = tmpVal/86400;

            tmpVal -= tmpDays*86400;
            tmpHours = tmpVal/3600;

            tmpVal -= tmpHours*3600;
            tmpMinutes = tmpVal/60;

            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_PLAYED_LVL, 1)->widget(0))->setText(Wt::WString::tr(TXT_CHAR_PLAYED_FMT).arg(tmpDays).arg(tmpHours).arg(tmpMinutes));
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_COST, 1)->widget(0))->setText(Misc::GetFormattedString("%u g", uint32(tmpRow->fields[7].GetUInt64()/GOLD)));
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_LAST_RESET_TIME, 1)->widget(0))->setText(tmpRow->fields[8].GetWString());

            tmpVal = tmpRow->fields[9].GetUInt64();
            tmpVal /= 30;   // one month for core = 30 days
            ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_ACTUAL_RESET_COST, 1)->widget(0))->setText(Misc::GetFormattedString("%u g", uint32(Misc::Character::CalculateTalentCost(tmpRow->fields[7].GetUInt64(), tmpVal)/GOLD)));

            if (IsDeletedCharacter(guid))
            {
                restoreCharacter->show();
                ((WText*)charBasicInfo->elementAt(CHARBASICINFO_SLOT_DELETION_TIME, 1)->widget(0))->setText(tmpRow->fields[10].GetWString());
                charBasicInfo->rowAt(CHARBASICINFO_SLOT_DELETION_TIME)->show();
            }
            else
            {
                restoreCharacter->hide();
                charBasicInfo->rowAt(CHARBASICINFO_SLOT_DELETION_TIME)->hide();
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Update Character Quest Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void CharacterInfoPage::UpdateCharacterQuestInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    switch (db.ExecutePQuery("SELECT cq.quest, qt.Name, qt.QuestLevel, cq.status, cq.rewarded, qt.Type, qt.MinLevel "
                            "FROM character_queststatus AS cq JOIN %s.quest_template AS qt ON cq.quest = qt.entry "
                            "WHERE guid = %u", SQL_WORLDDB, guid))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        }
        case DB_RESULT_EMPTY:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_EMPTY));
            return;
        }
        default:
        {
            db.Disconnect();
            std::list<DatabaseRow*> rows = db.GetRows();

            WTable * tmpTable = (WTable*)tabs->widget(CHAR_TAB_QUEST);

            int i = tmpTable->rowCount() - 1;

            for (; i > 0; --i)
                tmpTable->deleteRow(i);

            i = 1;

            DatabaseRow * tmpRow;
            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
            {
                tmpRow = *itr;

                if (tmpRow->fields[5].GetInt() == QUEST_TYPE_DAILY)
                    continue;

                tmpTable->elementAt(i, 0)->addWidget(new WText(Wt::WString::tr(TXT_QUEST_LINK_NAME_FMT).arg(tmpRow->fields[0].GetInt()).arg(tmpRow->fields[1].GetCString())));
                tmpTable->elementAt(i, 0)->setToolTip(Wt::WString::tr(TXT_QUEST_TOOLTIP_FMT).arg(tmpRow->fields[0].GetInt()).arg(tmpRow->fields[6].GetInt()), Wt::XHTMLText);
                tmpTable->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[2].GetWString()));
                tmpTable->elementAt(i, 2)->addWidget(new WText(Misc::Character::GetQuestStatus(tmpRow->fields[3].GetInt(), tmpRow->fields[4].GetBool())));
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Update Character Spell Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void CharacterInfoPage::UpdateCharacterSpellInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    switch (db.ExecutePQuery("SELECT spell, active, disabled "
                             "FROM character_spell "
                             "WHERE guid = '%u'", guid))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        }
        case DB_RESULT_EMPTY:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_EMPTY));
            return;
        }
        default:
        {
            db.Disconnect();
            std::list<DatabaseRow*> rows = db.GetRows();

            WTable * tmpTable = (WTable*)tabs->widget(CHAR_TAB_SPELL);

            int i = tmpTable->rowCount() - 1;

            for (; i > 0; --i)
                tmpTable->deleteRow(i);

            i = 1;

            DatabaseRow * tmpRow;
            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
            {
                tmpRow = *itr;

                tmpTable->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                tmpTable->elementAt(i, 1)->addWidget(new WText(spells[tmpRow->fields[0].GetUInt32()].name));
                tmpTable->elementAt(i, 2)->addWidget(new WText(Wt::WString::tr(tmpRow->fields[1].GetBool() ? TXT_GEN_YES : TXT_GEN_NO)));
                tmpTable->elementAt(i, 3)->addWidget(new WText(Wt::WString::tr(tmpRow->fields[2].GetBool() ? TXT_GEN_YES : TXT_GEN_NO)));
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Update Character Inventory Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void CharacterInfoPage::UpdateCharacterInventoryInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    switch (db.ExecutePQuery("SELECT ci.item_template, it.name, CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ', 15), ' ', -1) AS UNSIGNED) AS count "
                            "FROM character_inventory AS ci JOIN item_instance AS ii ON ci.item = ii.guid JOIN %s.item_template AS it ON ci.item_template = it.entry "
                            "WHERE ci.guid = %u", SQL_WORLDDB, guid))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        }
        case DB_RESULT_EMPTY:
        {
            return;
        }
        default:
        {
            db.Disconnect();
            std::list<DatabaseRow*> rows = db.GetRows();

            WTable * tmpTable = (WTable*)tabs->widget(CHAR_TAB_INVENTORY);

            int i = tmpTable->rowCount() - 1;

            for (; i > 0; --i)
                tmpTable->deleteRow(i);

            i = 1;

            DatabaseRow * tmpRow;
            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
            {
                tmpRow = *itr;

                tmpTable->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                tmpTable->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[1].GetWString()));
                tmpTable->elementAt(i, 2)->addWidget(new WText(tmpRow->fields[2].GetWString()));
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Update Character Friend Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void CharacterInfoPage::UpdateCharacterFriendInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    switch (db.ExecutePQuery("SELECT ch.name, note, flags, ch.online "
                            "FROM character_social AS cs JOIN characters AS ch ON cs.friend = ch.guid "
                            "WHERE cs.guid = '%u'", guid))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        }
        case DB_RESULT_EMPTY:
            return;
        default:
        {
            db.Disconnect();
            std::list<DatabaseRow*> rows = db.GetRows();

            WTable * tmpTable = (WTable*)tabs->widget(CHAR_TAB_FRIENDS);

            int i = tmpTable->rowCount() - 1;

            for (; i > 0; --i)
                tmpTable->deleteRow(i);

            i = 1;

            DatabaseRow * tmpRow;
            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;

                if (tmpRow->fields[2].GetUInt32() & SOCIAL_FLAG_FRIEND)
                {
                    tmpTable->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                    tmpTable->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[1].GetWString()));
                    tmpTable->elementAt(i, 2)->addWidget(new WText(Wt::WString::tr(tmpRow->fields[3].GetBool() ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE)));
                    ++i;
                }
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Update Character Mails widgets.
 ***********************************************/

void CharacterInfoPage::UpdateCharacterMailInfo(uint64 guid)
{
    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    // get all delivered mails
    switch (db.ExecutePQuery("SELECT mail.id, ch.name, mail.messageType, mail.stationery, mail.subject, FROM_UNIXTIME(mail.deliver_time), FROM_UNIXTIME(mail.expire_time), it.text, mail.money, mail.cod, mail.checked "
                            "FROM mail LEFT OUTER JOIN item_text as it on mail.itemTextId = it.id JOIN characters AS ch ON mail.sender = ch.guid "
                            "WHERE mail.receiver = '%u' AND mail.deliver_time < UNIX_TIMESTAMP()", guid))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            return;
        }
        case DB_RESULT_EMPTY:
            return;
        default:
        {
            std::list<DatabaseRow*> mailItems;
            std::list<DatabaseRow*> mails = db.GetRows();
            db.Disconnect();

            Database db2;
            if (db2.Connect(SERVER_DB_DATA, SQL_CHARDB))
            {
                // get all items attached to mails
                if (db2.ExecutePQuery("SELECT mail_id, item_template, name, CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ', 15), ' ', -1) AS UNSIGNED) AS count, item_guid "
                                        "FROM mail_items AS mi JOIN item_instance AS ii ON mi.item_guid = ii.guid LEFT OUTER JOIN world.item_template AS it ON mi.item_template = it.entry "
                                        "WHERE receiver = '%u'", guid) == DB_RESULT_ERROR)
                    charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));

                db2.Disconnect();
                mailItems = db2.GetRows();
            }

            // clear mail table if there is anything in it
            ClearMails();

            DatabaseRow * tmpRow;

            // prepare character mail informations
            for (std::list<DatabaseRow*>::const_iterator itr = mails.begin(); itr != mails.end(); ++itr)
            {
                // copy mail data
                MailInfo info(*itr);
                // and load items attached to current mail
                info.LoadItems(mailItems);

                // store prepared mail
                characterMails.push_back(info);
            }

            // fill character mail list
            int i = 1;
            for (std::vector<MailInfo>::const_iterator itr = characterMails.begin(); itr != characterMails.end(); ++itr, ++i)
            {
                const MailInfo & tmpMailInfo = *itr;

                mailList->elementAt(i, CHARMAILS_SLOT_FROM)->addWidget(new Wt::WText(tmpMailInfo.GetFrom()));
                mailList->elementAt(i, CHARMAILS_SLOT_TITLE)->addWidget(new Wt::WText(tmpMailInfo.GetSubject()));
                mailList->elementAt(i, CHARMAILS_SLOT_DATE)->addWidget(new Wt::WText(tmpMailInfo.GetDeliverTime()));
                mailList->elementAt(i, CHARMAILS_SLOT_EXPIRES)->addWidget(new Wt::WText(tmpMailInfo.GetExpireTime()));
                mailList->elementAt(i, CHARMAILS_SLOT_READED)->addWidget(new Wt::WText(Wt::WString::tr(tmpMailInfo.IsReaded() ? TXT_GEN_YES : TXT_GEN_NO)));

                // bind mail loading
                for (uint8 j = 0; j < CHARMAILS_SLOT_COUNT; ++j)
                    BindPreviewMail(mailList->elementAt(i, j)->clicked(), i-1); // i-1 'cause we have 1 header row
            }

            break;
        }
    }
}

/********************************************//**
 * \brief Loads mail preview.
 *
 * \param mailIdx   mail id in characterMails vector
 ***********************************************/

void CharacterInfoPage::PreviewMail(int mailIdx)
{
    if (mailIdx >= characterMails.size())
        return;

    mailPreviewFrom->setText(characterMails[mailIdx].GetFrom());
    mailPreviewExpire->setText(characterMails[mailIdx].GetExpireTime());
    mailPreviewSubject->setText(characterMails[mailIdx].GetSubject());
    mailPreviewBody->setText(characterMails[mailIdx].GetBody());
}

/********************************************//**
 * \brief Clears informations in widgets.
 *
 * Clears informations in widgets when current account level is too low.
 *
 ***********************************************/

void CharacterInfoPage::ClearPage()
{
    Misc::Console(DEBUG_CODE, "\nCharacterInfoPage::ClearPage()\n");

    clear();

    needCreation = true;
    indexToCharInfo.clear();
}

/********************************************//**
 * \brief Clears mails from character mail list, recreates header for table and also resets current mail preview.
 ***********************************************/

void CharacterInfoPage::ClearMails()
{
    if (!mailList)
        return;

    mailList->clear();
    mailList->setHeaderCount(1);

    mailList->elementAt(0, CHARMAILS_SLOT_FROM)->addWidget(new Wt::WText(Wt::WString::tr(TXT_MAIL_FROM)));
    mailList->elementAt(0, CHARMAILS_SLOT_TITLE)->addWidget(new Wt::WText(Wt::WString::tr(TXT_MAIL_SUBJECT)));
    mailList->elementAt(0, CHARMAILS_SLOT_DATE)->addWidget(new Wt::WText(Wt::WString::tr(TXT_MAIL_DATE)));
    mailList->elementAt(0, CHARMAILS_SLOT_EXPIRES)->addWidget(new Wt::WText(Wt::WString::tr(TXT_MAIL_EXPIRES)));
    mailList->elementAt(0, CHARMAILS_SLOT_READED)->addWidget(new Wt::WText(Wt::WString::tr(TXT_MAIL_READED)));

    characterMails.clear();

    mailPreviewFrom->setText("");
    mailPreviewExpire->setText("");
    mailPreviewSubject->setText("");
    mailPreviewBody->setText("");
}

void CharacterInfoPage::SelectionChanged(int selected)
{
    if (selected >= 0 && selected < charList->count())
    {
        std::map<int, CharInfo>::const_iterator tmpItr = indexToCharInfo.find(selected);
        if (tmpItr != indexToCharInfo.end())
            UpdateInformations(tmpItr->second.guid);
    }
}

bool CharacterInfoPage::IsDeletedCharacter(const uint64 & guid)
{
    for (std::map<int, CharInfo>::const_iterator itr = indexToCharInfo.begin(); itr != indexToCharInfo.end(); ++itr)
        if (itr->second.guid == guid)
            return itr->second.deleted;

    return false;
}

void CharacterInfoPage::RestoreCharacter()
{
    if (restoring)
        return;

    if (session->banned)
    {
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_NOT_WHILE_BANNED));
        return;
    }

    restoring = true;

    std::map<int, CharInfo>::iterator tmpItr = indexToCharInfo.find(charList->currentIndex());
    if (tmpItr == indexToCharInfo.end())    // check if character on that index exists
    {
        restoring = false;
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_CHARACTER_NOT_FOUND));
        return;
    }

    CharInfo tmpCharInfo = tmpItr->second;
    if (!IsDeletedCharacter(tmpCharInfo))    // check if character is deleted one
    {
        restoring = false;
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_CHARACTER_NOT_FOUND));
        return;
    }

    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        restoring = false;
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    if (db.ExecutePQuery("SELECT Count(*) FROM characters WHERE account = '%u'", tmpCharInfo.account) == DB_RESULT_ERROR)
    {
        restoring = false;
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    uint32 charactersCount = db.GetRow()->fields[0].GetUInt32();

    if (charactersCount >= MAX_CHARS_ON_ACCOUNT)
    {
        restoring = false;
        charPageInfo->setText(Wt::WString::tr(TXT_ERROR_TO_MUCH_CHARACTERS));
        return;
    }

    std::string escapedName = db.EscapeString(tmpCharInfo.name);

    switch (db.ExecutePQuery("SELECT guid FROM characters WHERE name = '%s'", escapedName.c_str()))
    {
        case DB_RESULT_ERROR:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        }
        case DB_RESULT_EMPTY:
        {
            bool sameFaction = true;

            #ifndef ALLOW_TWO_SIDE_ACCOUNTS
            ConflictSide tmpConflictSide = Misc::Character::GetSide(tmpCharInfo.race);
            for (std::map<int, CharInfo>::const_iterator itr = indexToCharInfo.begin(); itr != indexToCharInfo.end(); ++itr)
            {
                if (!itr->second.deleted)
                {
                    if (tmpConflictSide != Misc::Character::GetSide(itr->second.race))
                    {
                        sameFaction = false;
                        break;
                    }
                }
            }
            #endif

            if (sameFaction)
            {
                if (db.ExecutePQuery("UPDATE characters SET name = '%s', account = '%u' WHERE guid = '%u'", escapedName.c_str(), tmpCharInfo.account, tmpCharInfo.guid) != DB_RESULT_ERROR)
                {
                    db.ExecutePQuery("DELETE FROM deleted_chars WHERE char_guid = '%u'", tmpCharInfo.guid);

                    if (db.SelectDatabase(SQL_REALMDB))
                        db.ExecutePQuery("UPDATE realmcharacters SET numchars = '%u' WHERE account = '%u' AND realmid = 1", charactersCount + 1, tmpCharInfo.account);

                    tmpItr->second.deleted = false;
                    charList->setItemText(tmpItr->first, tmpItr->second.name);

                    charPageInfo->setText(Wt::WString::tr(TXT_CHAR_RESTORED));

                    restoreCharacter->hide();
                    charBasicInfo->rowAt(CHARBASICINFO_SLOT_DELETION_TIME)->hide();

                    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
                    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '%s')", tmpCharInfo.account, session->sessionIp.toUTF8().c_str(), TXT_ACT_CHARACTER_RESTORE, escapedName.c_str());
                }
                else
                    charPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            }
            else
                charPageInfo->setText(tr(TXT_ERROR_FACTION_MISMATCH));

            break;
        }
        default:
        {
            charPageInfo->setText(Wt::WString::tr(TXT_ERROR_CHARACTER_NAME_EXISTS));
            break;
        }
    }

    restoring = false;
}

void CharacterInfoPage::RebuildCharList()
{
    int currIndex = charList->currentIndex();

    charList->clear();

    for (std::map<int, CharInfo>::const_iterator itr = indexToCharInfo.begin(); itr != indexToCharInfo.end(); ++itr)
        charList->insertItem(itr->first, (itr->second.deleted ? "[Del] " : "") + itr->second.name);

    if (currIndex >= 0 && currIndex < charList->count())
        charList->setCurrentIndex(currIndex);
}

void CharacterInfoPage::LoadSpells()
{
    if (spellsLoaded)
        return;

    Database db;
    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        return;

    db.ExecuteQuery("SELECT entry, name FROM spells");

    std::list<DatabaseRow*> rows = db.GetRows();

    uint32 tmpEntry;
    for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
    {
        tmpEntry = (*itr)->fields[0].GetUInt32();
        spells[tmpEntry].entry = tmpEntry;
        spells[tmpEntry].name = (*itr)->fields[1].GetWString();
    }

    spellsLoaded = true;
}

void CharacterInfoPage::BindPreviewMail(Wt::EventSignal<Wt::WMouseEvent>& signal, int mailIdx)
{
    signal.connect(boost::bind(&CharacterInfoPage::PreviewMail, this, mailIdx));
}

/********************************************//**
 * \}
 * \}
 ***********************************************/
