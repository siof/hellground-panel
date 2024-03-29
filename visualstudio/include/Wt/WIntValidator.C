/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>

#include "Wt/WIntValidator"
#include "Wt/WString"
#include "Wt/WApplication"

#include "JavaScriptLoader.h"
#include "EscapeOStream.h"

#ifndef WT_DEBUG_JS
#include "js/WIntValidator.min.js"
#endif

namespace Wt {

WIntValidator::WIntValidator(WObject *parent)
  : WValidator(parent),
    bottom_(std::numeric_limits<int>::min()),
    top_(std::numeric_limits<int>::max())
{ }

WIntValidator::WIntValidator(int bottom, int top, WObject *parent)
  : WValidator(parent),
    bottom_(bottom),
    top_(top)
{ }

void WIntValidator::setBottom(int bottom)
{
  if (bottom != bottom_) {
    bottom_ = bottom;
    repaint();
  }
}

void WIntValidator::setTop(int top)
{
  if (top != top_) {
    top_ = top;
    repaint();
  }
}

void WIntValidator::setRange(int bottom, int top)
{
  setBottom(bottom);
  setTop(top);
}

void WIntValidator::setInvalidNotANumberText(const WString& text)
{
  nanText_ = text;
  repaint();
}

WString WIntValidator::invalidNotANumberText() const
{
  if (!nanText_.empty())
    return nanText_;
  else
    return WString::tr("Wt.WIntValidator.NotAnInteger");
}

void WIntValidator::setInvalidTooSmallText(const WString& text)
{
  tooSmallText_ = text;
  repaint();
}

WString WIntValidator::invalidTooSmallText() const
{
  if (!tooSmallText_.empty()) {
    WString s = tooSmallText_;
    s.arg(bottom_).arg(top_);
    return s;
  } else
    if (bottom_ == std::numeric_limits<int>::min())
      return WString();
    else
      if (top_ == std::numeric_limits<int>::max())
	return WString::tr("Wt.WIntValidator.TooSmall").arg(bottom_);
      else
	return WString::tr("Wt.WIntValidator.BadRange").arg(bottom_).arg(top_);
}

void WIntValidator::setInvalidTooLargeText(const WString& text)
{
  tooLargeText_ = text;
  repaint();
}

WString WIntValidator::invalidTooLargeText() const
{
  if (!tooLargeText_.empty()) {
    WString s = tooLargeText_;
    s.arg(bottom_).arg(top_);
    return s;
  } else
    if (top_ == std::numeric_limits<int>::max())
      return WString();
    else
      if (bottom_ == std::numeric_limits<int>::min())
	return WString::tr("Wt.WIntValidator.TooLarge").arg(top_);
      else
	return WString::tr("Wt.WIntValidator.BadRange").arg(bottom_).arg(top_);
}

WValidator::State WIntValidator::validate(WT_USTRING& input) const
{
  std::string text = input.toUTF8();

  if (isMandatory()) {
    if (text.empty())
      return InvalidEmpty;
  } else {
    if (text.empty())
      return Valid;
  }

  try {
    int i = boost::lexical_cast<int>(text);

    if ((i >= bottom_) && (i <= top_))
      return Valid;
    else
      return Invalid;
  } catch (boost::bad_lexical_cast& e) {
    return Invalid;
  }
}

void WIntValidator::loadJavaScript(WApplication *app)
{
  const char *THIS_JS = "js/WIntValidator.js";

  if (!app->javaScriptLoaded(THIS_JS)) {
    LOAD_JAVASCRIPT(app, THIS_JS, "WIntValidator", wtjs1);
    app->setJavaScriptLoaded(THIS_JS);
  }
}

std::string WIntValidator::javaScriptValidate() const
{
  loadJavaScript(WApplication::instance());

  SStream js;

  js << "new " WT_CLASS ".WIntValidator("
     << (isMandatory() ? "true" : "false") << ",";

  if (bottom_ != std::numeric_limits<int>::min())
    js << bottom_;
  else
    js << "null";

  js << ',';

  if (top_ != std::numeric_limits<int>::max())
    js << top_;
  else
    js << "null";

  js << ',' << invalidBlankText().jsStringLiteral()
     << ',' << invalidNotANumberText().jsStringLiteral()
     << ',' << invalidTooSmallText().jsStringLiteral()
     << ',' << invalidTooLargeText().jsStringLiteral()
     << ");";

  return js.str();
}

std::string WIntValidator::inputFilter() const
{
  return "[-+0-9]";
}

#ifndef WT_TARGET_JAVA
void WIntValidator::createExtConfig(std::ostream& config) const
{
  config << ",allowDecimals:false";

  if (bottom_ >= 0)
    config << ",allowNegative:false";
  if (bottom_ != std::numeric_limits<int>::min())
    config << ",minValue:" << bottom_;
  if (top_ != std::numeric_limits<int>::max())
    config << ",maxValue:" << top_;

  if (!tooSmallText_.empty())
    config << ",minText:" << tooSmallText_.jsStringLiteral();
  if (!tooLargeText_.empty())
    config << ",maxText:" << tooLargeText_.jsStringLiteral();
  if (!nanText_.empty())
    config << ",nanText:" << nanText_.jsStringLiteral();

  WValidator::createExtConfig(config);
}
#endif //WT_TARGET_JAVA

}
