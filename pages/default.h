#ifndef DEFAULT_H_INCLUDED
#define DEFAULT_H_INCLUDED

#include "../defines.h"

class DefaultPage : public WContainerWidget
{
public:
    DefaultPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~DefaultPage() {}

private:
    SessionInfo * session;
};

#endif // DEFAULT_H_INCLUDED
