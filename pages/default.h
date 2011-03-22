#ifndef DEFAULT_H_INCLUDED
#define DEFAULT_H_INCLUDED

#include "../defines.h"

class DefaultPage : public WContainerWidget
{
public:
    DefaultPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~DefaultPage() {}

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
private:
    SessionInfo * session;
    void ShowText();
};

#endif // DEFAULT_H_INCLUDED
