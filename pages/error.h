#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "../defines.h"

class ErrorPage : public WContainerWidget
{
public:
    ErrorPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~ErrorPage() {}

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
    void SetErrorMsg(int error);
    void SetAdditionalErrorMsg(const char * str);
    void SetAdditionalErrorMsg(std::string str);
private:
    void ShowError();

    SessionInfo * session;
    std::string errormsg;
    std::string additionalMsg;
};

#endif // ERROR_H_INCLUDED
