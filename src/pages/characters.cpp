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
 * \{
 *
 * \file characters.cpp
 * This file contains code needed to show player characters informations.
 *
 ***********************************************/

#include "characters.h"
#include "../database.h"

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
    setContentAlignment(AlignCenter|AlignTop);

    pageInfoSlots[CHARINFO_SLOT_INFO].SetLabel(sess, TXT_CHARACTER_PAGE_INFO);
    addWidget(pageInfoSlots[CHARINFO_SLOT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(new WText("", this));
    addWidget(pageInfoSlots[CHARINFO_SLOT_ADDINFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    pageInfoSlots[CHARINFO_SLOT_CHARACTERS].SetLabel(sess, TXT_LBL_CHAR_LIST);
    addWidget(pageInfoSlots[CHARINFO_SLOT_CHARACTERS].GetLabel());
    charList = new WComboBox(this);
    charList->activated().connect(this, &CharacterInfoPage::SelectionChanged);
    pageInfoSlots[CHARINFO_SLOT_CHARACTERS].SetWidget(charList);
    addWidget(new WBreak());
    addWidget(new WBreak());

    tabs = new WTabWidget();
    tabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);
    addWidget(tabs);

    needCreation = true;
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
    if (isHidden() || isDisabled())
        return;

    // only logged in players can visit this page so there is no need to create/update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
        {
            needCreation = false;

            tabs->addTab(CreateCharacterBasicInfo(), session->GetText(TXT_LBL_CHAR_TAB_INFO), WTabWidget::PreLoading);
            tabs->addTab(CreateCharacterQuestInfo(), session->GetText(TXT_LBL_CHAR_TAB_QUEST), WTabWidget::PreLoading);
            tabs->addTab(CreateCharacterSpellInfo(), session->GetText(TXT_LBL_CHAR_TAB_SPELL), WTabWidget::PreLoading);
            tabs->addTab(CreateCharacterInventoryInfo(), session->GetText(TXT_LBL_CHAR_TAB_INVENTORY), WTabWidget::PreLoading);

            charList->clear();
            indexToCharInfo.clear();
            Database db;
            if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
            {
                pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
                return;
            }

            int index = 0;
            switch (db.ExecutePQuery("SELECT guid, account, name, race FROM characters WHERE account = '%u'", session->accid))
            {
                case DB_RESULT_ERROR:
                    pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
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

            if (db.ExecutePQuery("SELECT char_guid, acc, race, oldname, date "
                                 "FROM deleted_chars JOIN characters ON deleted_chars.char_guid = characters.guid "
                                 "WHERE acc = '%u'", session->accid) > DB_RESULT_EMPTY)
            {

                std::list<DatabaseRow*> rows = db.GetRows();
                DatabaseRow * tmpRow;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
                {
                    tmpRow = *itr;

                    CharInfo tmpCharInfo(tmpRow->fields[0].GetUInt64(), tmpRow->fields[1].GetUInt32(), tmpRow->fields[2].GetWString(),  tmpRow->fields[3].GetInt(), true, tmpRow->fields[4].GetWString());

                    indexToCharInfo[index] = tmpCharInfo;
                    charList->insertItem(index++, "[Del] " + tmpCharInfo.name);
                }
            }

            db.Disconnect();

            // i think there is no need to update this informations
            if (charList->count() > 0)
            {
                charList->setCurrentIndex(0);
                std::map<int, CharInfo>::const_iterator tmpItr = indexToCharInfo.find(charList->currentIndex());
                if (tmpItr != indexToCharInfo.end())
                    UpdateInformations(tmpItr->second.guid);
            }
        }

        UpdateTextWidgets();
    }
    else
        ClearPage();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update text widgets.
 *
 * All text label widgets in all slots from current tab
 * will be updated, so if player will change language
 * then automagically labels should change too ;)
 *
 ***********************************************/

void CharacterInfoPage::UpdateTextWidgets()
{
    int i;
    for (i = 0; i < CHARINFO_SLOT_COUNT; ++i)
        pageInfoSlots[i].UpdateLabel(session);

    int count = tabs->count();

    for (i = 0; i < count; ++i)
        tabs->setTabText(i, session->GetText(TXT_LBL_CHAR_TAB_INFO + i));

    for (i = 0; i < CHARBASICINFO_SLOT_COUNT; ++i)
        basicInfoSlots[i].UpdateLabel(session);

    for (i = 0; i < CHARQUESTINFO_SLOT_COUNT; ++i)
        questInfoSlots[i].UpdateLabel(session);

    for (i = 0; i < CHARSPELLINFO_SLOT_COUNT; ++i)
        spellInfoSlots[i].UpdateLabel(session);

    for (i = 0; i < CHARINVINFO_SLOT_COUNT; ++i)
        inventoryInfoSlots[i].UpdateLabel(session);
}

/********************************************//**
 * \brief Update informations.
 *
 * Informations from current selected tab will be updated.
 *
 ***********************************************/

void CharacterInfoPage::UpdateInformations(uint64 guid)
{
    if (guid == 0)
        return;

    UpdateCharacterBasicInfo(guid);
    UpdateCharacterQuestInfo(guid);
    UpdateCharacterSpellInfo(guid);
    UpdateCharacterInventoryInfo(guid);
}

/********************************************//**
 * \brief Create informations widgets.
 *
 * Create widgets for each slot and fills them with informations.
 * This should be done only once for player.
 *
 ***********************************************/

WContainerWidget * CharacterInfoPage::CreateCharacterBasicInfo()
{
    WContainerWidget * tmpContainer = new WContainerWidget();

    basicInfoSlots[CHARBASICINFO_SLOT_LEVEL].SetAll(session, TXT_LBL_CHAR_LVL, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_RACE].SetAll(session, TXT_LBL_CHAR_RACE, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_CLASS].SetAll(session, TXT_LBL_CHAR_CLASS, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_NAME].SetAll(session, TXT_LBL_CHAR_NAME, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_ONLINE].SetAll(session, TXT_LBL_CHAR_ONLINE, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_PLAYED_TOT].SetAll(session, TXT_LBL_CHAR_TOTAL_PLAYED, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_PLAYED_LVL].SetAll(session, TXT_LBL_CHAR_LVL_PLAYED, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_RESET_COST].SetAll(session, TXT_LBL_CHAR_TALENT_RESET_COST, new WText(""), 1);
    basicInfoSlots[CHARBASICINFO_SLOT_RESET_TIME].SetAll(session, TXT_LBL_CHAR_TALENT_RESET_TIME, new WText(""), 1);

    int tmpCount;
    WWidget * tmpWidget;
    // add widgets to page
    for (int i = 0; i < CHARBASICINFO_SLOT_COUNT; ++i)
    {
        tmpWidget = basicInfoSlots[i].GetLabel();
        if (tmpWidget)
            tmpContainer->addWidget(tmpWidget);

        tmpWidget = basicInfoSlots[i].GetWidget();
        if (tmpWidget)
            tmpContainer->addWidget(tmpWidget);

        tmpCount = basicInfoSlots[i].GetBreakCount();

        for (int j = 0; j < tmpCount; ++j)
            tmpContainer->addWidget(new WBreak());
    }

    tmpContainer->addWidget(new WBreak());
    tmpContainer->addWidget(new WBreak());

    if (restoreCharacter)
        delete restoreCharacter;

    restoreCharacter = new WPushButton(session->GetText(TXT_BTN_CHARACTER_RESTORE), tmpContainer);
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

    questInfoSlots[CHARQUESTINFO_SLOT_ID].SetLabel(session, TXT_LBL_QUEST_ID);
    questInfoSlots[CHARQUESTINFO_SLOT_NAME].SetLabel(session, TXT_LBL_QUEST_NAME);
    questInfoSlots[CHARQUESTINFO_SLOT_LEVEL].SetLabel(session, TXT_LBL_QUEST_LVL);
    questInfoSlots[CHARQUESTINFO_SLOT_STATUS].SetLabel(session, TXT_LBL_QUEST_STATUS);

    int i;
    for (i = 0; i < CHARQUESTINFO_SLOT_COUNT; ++i)
        tmpQuest->elementAt(0, i)->addWidget(questInfoSlots[i].GetLabel());

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
    WTable * tmpSpell = new WTable();

    tmpSpell->setHeaderCount(1);

    spellInfoSlots[CHARSPELLINFO_SLOT_ID].SetLabel(session, TXT_LBL_SPELL_ID);
    spellInfoSlots[CHARSPELLINFO_SLOT_NAME].SetLabel(session, TXT_LBL_SPELL_NAME);
    spellInfoSlots[CHARSPELLINFO_SLOT_ACTIVE].SetLabel(session, TXT_LBL_SPELL_ACTIVE);
    spellInfoSlots[CHARSPELLINFO_SLOT_DISABLED].SetLabel(session, TXT_LBL_SPELL_DISABLED);

    int i;
    for (i = 0; i < CHARSPELLINFO_SLOT_COUNT; ++i)
        tmpSpell->elementAt(0, i)->addWidget(spellInfoSlots[i].GetLabel());

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

    inventoryInfoSlots[CHARINVINFO_SLOT_ID].SetLabel(session, TXT_LBL_ITEM_ID);
    inventoryInfoSlots[CHARINVINFO_SLOT_NAME].SetLabel(session, TXT_LBL_ITEM_NAME);

    int i;
    for (i = 0; i < CHARINVINFO_SLOT_COUNT; ++i)
        tmpInv->elementAt(0, i)->addWidget(inventoryInfoSlots[i].GetLabel());

    return tmpInv;
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
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    switch (db.ExecutePQuery("SELECT level, race, class, name, online, totaltime, leveltime, resettalents_cost, FROM_UNIXTIME(resettalents_time) "
                             "FROM characters "
                             "WHERE guid = '%u'", guid))
    {
        case DB_RESULT_ERROR:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            return;
        case DB_RESULT_EMPTY:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_EMPTY);
            return;
        default:
            db.Disconnect();
            DatabaseRow * tmpRow = db.GetRow();

            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_LEVEL].GetWidget())->setText(tmpRow->fields[0].GetWString());
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_RACE].GetWidget())->setText(GetRaceName(session, tmpRow->fields[1].GetInt()));
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_CLASS].GetWidget())->setText(GetClassName(session, tmpRow->fields[2].GetInt()));
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_NAME].GetWidget())->setText(tmpRow->fields[3].GetWString());
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_ONLINE].GetWidget())->setText(session->GetText(tmpRow->fields[4].GetBool() ? TXT_ONLINE : TXT_OFFLINE));

            uint64 tmpVal = tmpRow->fields[5].GetUInt64();
            uint32 tmpDays = tmpVal/86400;

            tmpVal -= tmpDays * 86400;
            uint32 tmpHours = tmpVal/3600;

            tmpVal -= tmpHours * 3600;
            uint32 tmpMinutes = tmpVal/60;

            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_PLAYED_TOT].GetWidget())->setText(GetFormattedString(session->GetText(TXT_CHARACTER_PLAYED_FMT).toUTF8().c_str(), tmpDays, tmpHours, tmpMinutes));

            tmpVal = tmpRow->fields[6].GetUInt64();
            tmpDays = tmpVal/86400;

            tmpVal -= tmpDays*86400;
            tmpHours = tmpVal/3600;

            tmpVal -= tmpHours*3600;
            tmpMinutes = tmpVal/60;

            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_PLAYED_LVL].GetWidget())->setText(GetFormattedString(session->GetText(TXT_CHARACTER_PLAYED_FMT).toUTF8().c_str(), tmpDays, tmpHours, tmpMinutes));
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_RESET_COST].GetWidget())->setText(GetFormattedString("%lu g", uint64(tmpRow->fields[7].GetUInt64()/10000)));
            ((WText*)basicInfoSlots[CHARBASICINFO_SLOT_RESET_TIME].GetWidget())->setText(tmpRow->fields[8].GetWString());

            if (IsDeletedCharacter(guid))
                restoreCharacter->show();

            break;
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
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    switch (db.ExecutePQuery("SELECT cq.quest, qt.Title, qt.QuestLevel, cq.status, cq.rewarded "
                            "FROM character_queststatus AS cq JOIN %s.quest_template AS qt ON cq.quest = qt.entry "
                            "WHERE guid = %u", SQL_WORLDDB, guid))
    {
        case DB_RESULT_ERROR:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            return;
        case DB_RESULT_EMPTY:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_EMPTY);
            return;
        default:
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

                tmpTable->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                tmpTable->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[1].GetWString()));
                tmpTable->elementAt(i, 2)->addWidget(new WText(tmpRow->fields[2].GetWString()));
                tmpTable->elementAt(i, 3)->addWidget(new WText(GetQuestStatus(session, tmpRow->fields[3].GetInt(), tmpRow->fields[4].GetBool())));
            }

            break;
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
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    switch (db.ExecutePQuery("SELECT spell, active, disabled "
                             "FROM character_spell "
                             "WHERE guid = '%u'", guid))
    {
        case DB_RESULT_ERROR:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            return;
        case DB_RESULT_EMPTY:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_EMPTY);
            return;
        default:
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
                tmpTable->elementAt(i, 2)->addWidget(new WText(session->GetText(tmpRow->fields[1].GetBool() ? TXT_YES : TXT_NO)));
                tmpTable->elementAt(i, 3)->addWidget(new WText(session->GetText(tmpRow->fields[2].GetBool() ? TXT_YES : TXT_NO)));
            }

            break;
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
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    switch (db.ExecutePQuery("SELECT ci.item_template, it.name "
                            "FROM character_inventory AS ci JOIN %s.item_template AS it ON ci.item_template = it.entry "
                            "WHERE guid = %u", SQL_WORLDDB, guid))
    {
        case DB_RESULT_ERROR:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            return;
        case DB_RESULT_EMPTY:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_EMPTY);
            return;
        default:
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
            }

            break;
    }
}

/********************************************//**
 * \brief Clears informations in widgets.
 *
 * Clears informations in widgets when current account level is too low.
 *
 ***********************************************/

void CharacterInfoPage::ClearPage(bool alsoCharList)
{
    console(DEBUG_CODE, "\nCharacterInfoPage::ClearPage()\n");

    WWidget * tmpWid;
    int i;

    for (i = CHAR_TAB_COUNT - 1; i >= 0; --i)
    {
        if (tmpWid = tabs->widget(i))
        {
            tabs->removeTab(tmpWid);

            switch (i)
            {
                case CHAR_TAB_BASIC:
                    ((WContainerWidget*)tmpWid)->clear();
                    delete tmpWid;
                    break;
                case CHAR_TAB_QUEST:
                case CHAR_TAB_SPELL:
                case CHAR_TAB_INVENTORY:
                    ((WTable*)tmpWid)->clear();
                    delete tmpWid;
                    break;
                default:
                    break;
            }
        }
    }

    for (i = 0; i < CHARBASICINFO_SLOT_COUNT; ++i)
        basicInfoSlots[i].Clear();

    for (i = 0; i < CHARQUESTINFO_SLOT_COUNT; ++i)
        questInfoSlots[i].Clear();

    for (i = 0; i < CHARSPELLINFO_SLOT_COUNT; ++i)
        spellInfoSlots[i].Clear();

    for (i = 0; i < CHARINVINFO_SLOT_COUNT; ++i)
        inventoryInfoSlots[i].Clear();

    if (alsoCharList)
    {
        needCreation = true;
        charList->clear();
        indexToCharInfo.clear();
    }
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
    if (!guid)
        return false;

    std::map<int, CharInfo>::const_iterator tmpItr = indexToCharInfo.find(guid);
    if (tmpItr != indexToCharInfo.end())
        return tmpItr->second.deleted;

    return false;
}

void CharacterInfoPage::RestoreCharacter()
{
    if (restoring)
        return;

    restoring = true;

    std::map<int, CharInfo>::iterator tmpItr = indexToCharInfo.find(charList->currentIndex());
    if (tmpItr == indexToCharInfo.end())    // check if character on that index exists
    {
        restoring = false;
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_ERROR_CHARACTER_NOT_FOUND);
        return;
    }

    CharInfo tmpCharInfo = tmpItr->second;
    if (!IsDeletedCharacter(tmpCharInfo))    // check if character is deleted one
    {
        restoring = false;
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_ERROR_CHARACTER_NOT_FOUND);
        return;
    }

    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        restoring = false;
        pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    std::string escapedName = db.EscapeString(tmpCharInfo.name);

    switch (db.ExecutePQuery("SELECT guid FROM characters WHERE name = '%s'", escapedName.c_str()))
    {
        case DB_RESULT_ERROR:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            break;
        case DB_RESULT_EMPTY:
        {
            bool sameFaction = true;

            #ifndef ALLOW_TWO_SIDE_ACCOUNTS
            ConflictSide tmpConflictSide = GetSide(tmpCharInfo.race);
            for (std::map<int, CharInfo>::const_iterator itr = indexToCharInfo.begin(); itr != indexToCharInfo.end(); ++itr)
            {
                if (tmpConflictSide != GetSide(itr->second.race))
                {
                    sameFaction = false;
                    break;
                }
            }
            #endif

            if (sameFaction)
            {
                if (db.ExecutePQuery("UPDATE characters SET name = '%s', account = '%u' WHERE guid = '%u'", escapedName.c_str(), tmpCharInfo.account, tmpCharInfo.guid) != DB_RESULT_ERROR)
                {
                    db.ExecutePQuery("DELETE FROM deleted_chars WHERE char_guid = '%u'", tmpCharInfo.guid);
                    tmpItr->second.deleted = false;
                    RebuildCharList();

                    pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_CHARACTER_RESTORED);
                }
                else
                    pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            }
            else
                pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_ERROR_FACTION_MISMATCH);

            break;
        }
        default:
            pageInfoSlots[CHARINFO_SLOT_ADDINFO].SetLabel(session, TXT_ERROR_CHARACTER_NAME_EXISTS);
            break;
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

/********************************************//**
 * \}
 * \}
 ***********************************************/
