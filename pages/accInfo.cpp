#include "accInfo.h"

AccountInfoPage::AccountInfoPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
}

void AccountInfoPage::refresh()
{

    WContainerWidget::refresh();
}
