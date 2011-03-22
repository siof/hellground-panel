#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

// standard includes for most files
#include <WString>
#include <WBreak>
#include <WContainerWidget>
#include <WLineEdit>
#include <WPushButton>
#include <WText>
#include <WImage>
#include <WTable>

#include <cstdio>

using namespace Wt;

typedef uint64_t uint64;

enum Lang
{
    LANG_PL = 0,
    LANG_EN
};

enum AccountLevel
{
    LVL_PLAYER      = 0,
    LVL_GM_TRIAL    = 1,
    LVL_GM_HELPER   = 2,
    LVL_GM_HEAD     = 3,
    LVL_ADM         = 4
};

struct SessionInfo
{
    SessionInfo() : login(""), accid(0), pass(""), email(""), language(LANG_PL), accLvl(LVL_PLAYER) {}
    ~SessionInfo() {}
    WString login;
    uint64 accid;
    WString pass;
    WString email;
    Lang language;
    AccountLevel accLvl;
};

#define SITE_TITLE  "HellGround Player's Panel"

#define SITE_NAME   "HellGround"

#endif // DEFINES_H_INCLUDED
