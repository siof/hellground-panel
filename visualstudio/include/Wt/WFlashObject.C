/*
 * Copyright (C) 2009 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include "Wt/WFlashObject"
#include "Wt/WAnchor"
#include "Wt/WApplication"
#include "Wt/WEnvironment"
#include "Wt/WImage"
#include "Wt/WLength"
#include "DomElement.h"
#include "Utils.h"

#include <boost/lexical_cast.hpp>
#include <sstream>

namespace {
  std::string toString(const Wt::WLength& length) 
  {
    if (length.isAuto())
      return "";
    else if (length.unit() == Wt::WLength::Percentage)
      return "100%";
    else
      return boost::lexical_cast<std::string>((int)length.toPixels()) + "px";
  }
}

namespace Wt {

WFlashObject::WFlashObject(const std::string& url,
                           WContainerWidget *parent)
  : WWebWidget(parent),
    url_(url),
    sizeChanged_(false),
    alternative_(0),
    ieRendersAlternative_(this, "IeAltnernative"),
    replaceDummyIeContent_(false)
{
  setInline(false);
  setAlternativeContent(new WAnchor("http://www.adobe.com/go/getflashplayer",
    new WImage("http://www.adobe.com/images/"
               "shared/download_buttons/get_flash_player.gif")));
  ieRendersAlternative_.connect(this, &WFlashObject::renderIeAltnerative);
}

WFlashObject::~WFlashObject()
{
}

void WFlashObject::setFlashParameter(const std::string &name,
    const WString &value)
{
  WString v = value;
  parameters_[name] = v;
}

void WFlashObject::setFlashVariable(const std::string &name,
    const WString &value)
{
  WString v = value;
  variables_[name] = v;
}

void WFlashObject::updateDom(DomElement& element, bool all)
{
  if (all) {
    //http://latrine.dgx.cz/how-to-correctly-insert-a-flash-into-xhtml
    DomElement *obj = DomElement::createNew(DomElement_OBJECT);

    if (isInLayout()) {
      // Layout-manager managed sizes need some CSS magic to display
      // correctly
      obj->setProperty(PropertyStylePosition, "absolute");
      obj->setProperty(PropertyStyleLeft, "0");
      obj->setProperty(PropertyStyleRight, "0");
      element.setProperty(PropertyStylePosition, "relative");
      std::stringstream ss;
      // Client-side auto-resize function
      ss <<
        """function(self, w, h) {"
        ""  "v=" + jsFlashRef() + ";"
        ""  "if(v){"
        ""    "v.setAttribute('width', w);"
        ""    "v.setAttribute('height', h);"
        ""  "}";
      if (alternative_) {
        ss <<
          """a=" + alternative_->jsRef() + ";"
          ""  "if(a && a." << WT_RESIZE_JS <<")"
          ""    "a." << WT_RESIZE_JS << "(a, w, h);";
      }
      ss
        <<"}";
      setJavaScriptMember(WT_RESIZE_JS, ss.str());
    }

    obj->setId(id() + "_flash");
    obj->setAttribute("type", "application/x-shockwave-flash");
    if (!wApp->environment().agentIsIElt(9)) {
      obj->setAttribute("data", url_);
    }
    // Width/height: Adobe says: must be present, and specified as pixels or
    // percentage. We noticed that when left blank, most players come up with
    // a reasonable default. All other units (not px or %) are converted to
    // px. When percentage units, the surrounding div will have the % set.
    // The flash movie is thus set to 100% in order to fill the space of the
    // div.
    // http://kb2.adobe.com/cps/127/tn_12701.html
    obj->setAttribute("width", toString(width()));
    obj->setAttribute("height", toString(height()));

    for(std::map<std::string, WString>::const_iterator i = parameters_.begin();
      i != parameters_.end(); ++i) {
        if (i->first != "flashvars") {
          DomElement *param = DomElement::createNew(DomElement_PARAM);
          param->setAttribute("name", i->first);
          param->setAttribute("value", i->second.toUTF8());
          obj->addChild(param);
        }
    }
    if (wApp->environment().agentIsIElt(9)) {
      obj->setAttribute("classid", "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000");
      // The next line is considered bad practice
      //obj->setAttribute("codebase",
      //"http://download.macromedia.com/pub/shockwave/cabs/flash/
      //swflash.cab#version=6,0,0,0");
      DomElement *param = DomElement::createNew(DomElement_PARAM);
      param->setAttribute("name", "movie");
      param->setAttribute("value", url_);
      obj->addChild(param);
    }
    if (variables_.size() > 0) {
      std::stringstream ss;
      for (std::map<std::string, WString>::const_iterator i = variables_.begin();
        i != variables_.end(); ++i) {
          if (i != variables_.begin())
            ss << "&";
          ss << Wt::Utils::urlEncode(i->first) << "="
            << Wt::Utils::urlEncode(i->second.toUTF8());
      }
      DomElement *param = DomElement::createNew(DomElement_PARAM);
      param->setAttribute("name", "flashvars");
      param->setAttribute("value", ss.str());
      obj->addChild(param);
    }
    if (alternative_) {
      // Internet explorer simply eliminates the inner elements if they are
      // not needed, causing JavaScript errors lateron. So we will only
      // render the alternative content if we know for sure that it is not
      // eliminated by IE (in practice, this is when Flash support is
      // not available). To do so, we'll render a dummy div, verify if that
      // dummy element is not eliminated, and if it's not, replace it by
      // a call to alternative_->createDomElement().
      if (wApp->environment().javaScript() &&
          wApp->environment().agentIsIElt(9)) {
        DomElement *dummyDiv = DomElement::createNew(DomElement_DIV);
        dummyDiv->setId(alternative_->id());
        // As if it ain't bad enough, the altnerative content is only
        // inserted in the DOM after 'a while', so we can't test for it with
        // a simple doJavaScript() call. Additionally, all scripting-alike
        // stuff inside the alternative content is ignored (script element,
        // onrenderstatechange, ...). So this 'style=...' is indeed a hack.
        // You can't put semicolons or curly braces inside the expression,
        // so we added a helper function.
        dummyDiv->setAttribute("style",
          "width: expression(" + wApp->javaScriptClass()
          + "._p_.ieAlternative(this));");
        obj->addChild(dummyDiv);
      } else {
        obj->addChild(alternative_->createSDomElement(wApp));
      }
    }
    element.addChild(obj);
  }

  WWebWidget::updateDom(element, all);
}

std::string WFlashObject::jsFlashRef() const
{
  return WT_CLASS ".getElement('" + id() + "_flash')";
}

void WFlashObject::getDomChanges(std::vector<DomElement *>& result,
                                 WApplication *app)
{
  WWebWidget::getDomChanges(result, app);
  if (sizeChanged_) {
    std::stringstream ss;
    ss << 
      ""  "var v=" << jsFlashRef() << ";"
      ""  "if(v){"
      ""    "v.setAttribute('width', " << toString(width()) << ");"
      ""    "v.setAttribute('height', " << toString(height()) << ");"
      ""  "}";
    WApplication::instance()->doJavaScript(ss.str());

    sizeChanged_ = false;
  }
  if (alternative_ && replaceDummyIeContent_) {
    DomElement *element =
      DomElement::getForUpdate(alternative_->id(), DomElement_DIV);
    element->replaceWith(alternative_->createSDomElement(app));
    result.push_back(element);
    replaceDummyIeContent_ = false;
  }

}

void WFlashObject::setAlternativeContent(WWidget *alternative)
{
  if (alternative_)
    delete alternative_;
  alternative_ = alternative;
  if (alternative_)
    addChild(alternative_);
}

void WFlashObject::resize(const WLength &width, const WLength &height)
{
  sizeChanged_ = true;
  WWebWidget::resize(width, height);
  repaint(Wt::RepaintPropertyAttribute);
}

DomElementType WFlashObject::domElementType() const
{
  return DomElement_DIV;
}
void WFlashObject::renderIeAltnerative()
{
  replaceDummyIeContent_ = true;
  repaint(Wt::RepaintInnerHtml);
}

}
