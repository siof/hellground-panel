#include "error.h"

ErrorPage::ErrorPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    ShowError();
}

void ErrorPage::refresh()
{
    ShowError();

    WContainerWidget::refresh();
}

void ErrorPage::SetErrorMsg(int error)
{
    switch (error)
    {
        case ERROR_DB_CONNECT:
            switch (session->language)
            {
                case LANG_EN:
                    errormsg = "Can't connect to database !";
                    break;
                case LANG_PL:
                default:
                    errormsg = "Błąd połączenia z bazą danych !";
                    break;
            }
            break;
        case ERROR_LOGIN:
            switch (session->language)
            {
                case LANG_EN:
                    errormsg = "Invalid login or password";
                    break;
                case LANG_PL:
                default:
                    errormsg = "Błędny login lub hasło";
                    break;
            }
            break;
        default:
            errormsg = "";
            break;
    }

    ShowError();
}

void ErrorPage::SetAdditionalErrorMsg(const char * str)
{
    if (!str)
        return;

    additionalMsg = str;
}

void ErrorPage::SetAdditionalErrorMsg(std::string str)
{
    SetAdditionalErrorMsg(str.c_str());
}

void ErrorPage::ShowError()
{
    if (count())
    {
        for (int i = count() - 1; i >= 0; --i)
        {
            WWidget * tmp = widget(i);
            removeWidget(tmp);
            delete tmp;
            tmp = NULL;
        }
    }

    addWidget(new WText(utf8(errormsg)));
    addWidget(new WBreak());
    addWidget(new WText(utf8(additionalMsg)));
}
