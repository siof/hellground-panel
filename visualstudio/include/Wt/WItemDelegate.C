/*
 * Copyright (C) 2009 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/WItemDelegate"

#include "Wt/WAbstractItemModel"
#include "Wt/WAnchor"
#include "Wt/WApplication"
#include "Wt/WCheckBox"
#include "Wt/WContainerWidget"
#include "Wt/WEnvironment"
#include "Wt/WImage"
#include "Wt/WModelIndex"
#include "Wt/WHBoxLayout"
#include "Wt/WText"

#include "Utils.h"

namespace Wt {

template <class Widget>
class IndexEdit : public Widget
{
public:
  IndexEdit(const WModelIndex& index)
    : index_(index)
  { }

  void setIndex(const WModelIndex& index) {
    index_ = index;
  }

  const WModelIndex& index() const {
    return index_;
  }

private:
  WModelIndex index_;
};

#ifdef WT_CNOR
class IndexCheckBox : public IndexEdit<WCheckBox>
{
public:
  IndexCheckBox(const WModelIndex& index);

  void setIndex(const WModelIndex& index);
  const WModelIndex& index();
};
#endif // WT_CNOR

WItemDelegate::WItemDelegate(WObject *parent)
  : WAbstractItemDelegate(parent)
{ }

void WItemDelegate::setTextFormat(const WT_USTRING& format)
{
  textFormat_ = format;
}

WWidget *WItemDelegate::update(WWidget *widget, const WModelIndex& index,
			       WFlags<ViewItemRenderFlag> flags)
{
  bool editing = widget && widget->find("t") == 0;

  if (flags & RenderEditing) {
    if (!editing) {
      widget = createEditor(index, flags);
      WInteractWidget *iw = dynamic_cast<WInteractWidget *>(widget);
      if (iw) {
	// Disable drag & drop and selection behaviour
	iw->mouseWentDown().preventPropagation();
	iw->clicked().preventPropagation();
      }
    }
  } else {
    if (editing)
      widget = 0;
  }

  WidgetRef widgetRef(widget);

  bool isNew = false;

  if (!(flags & RenderEditing)) {
    if (!widgetRef.w) {
      isNew = true;
      WText *t = new WText();
      t->setObjectName("t");
      if (index.isValid() && !(index.flags() & ItemIsXHTMLText))
	t->setTextFormat(PlainText);
      t->setWordWrap(true);
      widgetRef.w = t;
    }

    if (!index.isValid())
      return widgetRef.w;

    bool haveCheckBox = false;

    if (index.flags() & ItemIsUserCheckable) {
      boost::any checkedData = index.data(CheckStateRole);
      CheckState state = checkedData.empty() ? Unchecked
	: (checkedData.type() == typeid(bool) ?
	   (boost::any_cast<bool>(checkedData) ? Checked : Unchecked)
	   : (checkedData.type() == typeid(CheckState) ?
	      boost::any_cast<CheckState>(checkedData) : Unchecked));
      checkBox(widgetRef, index, true, index.flags() & ItemIsTristate)
	->setCheckState(state);
      haveCheckBox = true;
    } else if (!isNew)
      delete checkBox(widgetRef, index, false);

    std::string internalPath = asString(index.data(InternalPathRole)).toUTF8();
    std::string url = asString(index.data(UrlRole)).toUTF8();

    if (!internalPath.empty() || !url.empty()) {
      WAnchor *a = anchorWidget(widgetRef);

      if (!internalPath.empty())
	a->setRefInternalPath(internalPath);
      else
	a->setRef(url);
    }

    WText *t = textWidget(widgetRef);

    WString label = asString(index.data(), textFormat_);
    if (label.empty() && haveCheckBox)
      label = WString::fromUTF8(" ");
    t->setText(label);

    std::string iconUrl = asString(index.data(DecorationRole)).toUTF8();
    if (!iconUrl.empty()) {
      iconWidget(widgetRef, true)->setImageRef(iconUrl);
    } else if (!isNew)
      delete iconWidget(widgetRef, false);
  }

  WString tooltip = asString(index.data(ToolTipRole));
  if (!tooltip.empty() || !isNew)
    widgetRef.w->setToolTip(tooltip);

  WT_USTRING sc = asString(index.data(StyleClassRole));

  if (flags & RenderSelected)
    sc += WT_USTRING::fromUTF8(" Wt-selected");

  if (flags & RenderEditing)
    sc += WT_USTRING::fromUTF8(" Wt-delegate-edit");

  widgetRef.w->setStyleClass(sc);

  if (index.flags() & ItemIsDropEnabled)
    widgetRef.w->setAttributeValue("drop", WString::fromUTF8("true"));
  else
    if (!widgetRef.w->attributeValue("drop").empty())
      widgetRef.w->setAttributeValue("drop", WString::fromUTF8("f"));

  return widgetRef.w;
}

/*
 * Possible layouts:
 *  1) WText "t"
 * or
 *  2) WContainerWidget "o" ([WCheckbox "c"] [WImage "i"] [inv] WText "t")
 * or
 *  3) WAnchor "a" ([WImage "i"] [inv] WText "t")
 * or
 *  4) WContainerWidget "o" ([WCheckbox "c"] WAnchor "a" ([Image "i"] [inv] WText "t"))
 */

IndexCheckBox *WItemDelegate::checkBox(WidgetRef& w, const WModelIndex& index,
				      bool autoCreate, bool triState)
{
  IndexCheckBox *checkBox = dynamic_cast<IndexCheckBox *>(w.w->find("c"));

  if (!checkBox) {
    if (autoCreate) {
      IndexCheckBox * const result = checkBox = new IndexCheckBox(index);

      checkBox->setObjectName("c");
      checkBox->clicked().preventPropagation();

      WContainerWidget *wc = dynamic_cast<WContainerWidget *>(w.w->find("o"));
      if (!wc) {
	wc = new WContainerWidget();
	wc->setObjectName("o");
	w.w->setInline(true);
	w.w->setStyleClass(WString::Empty);

	/* We first remove to avoid reparenting warnings */
	WContainerWidget *p = dynamic_cast<WContainerWidget *>(w.w->parent());
	if (p)
	  p->removeWidget(w.w);

	wc->addWidget(w.w);
	w.w = wc;
      }
      
      wc->insertWidget(0, checkBox);
      checkBox->changed().connect
	(boost::bind(&WItemDelegate::onCheckedChange, this, result));
    } else
      return 0;
  }

  checkBox->setTristate(triState);

  return checkBox;
}

WText *WItemDelegate::textWidget(WidgetRef& w)
{
  return dynamic_cast<WText *>(w.w->find("t"));
}

WImage *WItemDelegate::iconWidget(WidgetRef& w, bool autoCreate)
{
  WImage *image = dynamic_cast<WImage *>(w.w->find("i"));
  if (image || !autoCreate)
    return image;

  WContainerWidget *wc = dynamic_cast<WContainerWidget *>(w.w->find("a"));
      
  if (!wc)
    wc = dynamic_cast<WContainerWidget *>(w.w->find("o"));

  if (!wc) {
    wc = new WContainerWidget();
    wc->setObjectName("o");
    wc->addWidget(w.w);
    w.w = wc;
  }

  image = new WImage();
  image->setObjectName("i");
  image->setStyleClass("icon");
  wc->insertWidget(wc->count() - 1, image);

  // IE does not want to center vertically without this:
  if (wApp->environment().agentIsIE()) {
    WImage *inv = new WImage(wApp->onePixelGifUrl());
    inv->setStyleClass("rh w0 icon");
    inv->resize(0, WLength::Auto);
    wc->insertWidget(wc->count() -1, inv);
  }

  return image;
}

WAnchor *WItemDelegate::anchorWidget(WidgetRef& w)
{
  WAnchor *anchor = dynamic_cast<WAnchor *>(w.w->find("a"));
  if (anchor)
    return anchor;

  anchor = new WAnchor();
  anchor->setObjectName("a");

  WContainerWidget *wc = dynamic_cast<WContainerWidget *>(w.w->find("o"));
  if (wc) {
    /*
     * Convert (2) -> (4)
     */
    int firstToMove = 0;

    WCheckBox *cb = dynamic_cast<WCheckBox *>(wc->widget(0));
    if (cb)
      firstToMove = 1;

    wc->insertWidget(firstToMove, anchor);

    while (wc->count() > firstToMove + 1) { 
      WWidget *c = wc->widget(firstToMove + 1);
      wc->removeWidget(c);
      anchor->addWidget(c);
    }
  } else {
    /*
     * Convert (1) -> (3)
     */
    anchor->addWidget(w.w);
    w.w = anchor;
  }

  return anchor;
}

void WItemDelegate::updateModelIndex(WWidget *widget, const WModelIndex& index)
{
  WidgetRef w(widget);

  if (index.flags() & ItemIsUserCheckable) {
    IndexCheckBox *cb = checkBox(w, index, false, false);
    if (cb)
      cb->setIndex(index);
  }
}

void WItemDelegate::onCheckedChange(IndexCheckBox *cb) const
{
  WAbstractItemModel *model
    = const_cast<WAbstractItemModel *>(cb->index().model());

  if (cb->isTristate())
    model->setData(cb->index(), boost::any(cb->checkState()), CheckStateRole);
  else
    model->setData(cb->index(), boost::any(cb->isChecked()), CheckStateRole);
}

WWidget *WItemDelegate::createEditor(const WModelIndex& index,
				     WFlags<ViewItemRenderFlag> flags) const
{
  WContainerWidget *const result = new WContainerWidget();
  result->setSelectable(true);

  WLineEdit *lineEdit = new WLineEdit();
  lineEdit->setText(asString(index.data(EditRole), textFormat_));
  lineEdit->enterPressed().connect
    (boost::bind(&WItemDelegate::doCloseEditor, this, result, true));
  lineEdit->escapePressed().connect
    (boost::bind(&WItemDelegate::doCloseEditor, this, result, false));
  lineEdit->escapePressed().preventPropagation();

  if (flags & RenderFocused)
    lineEdit->setFocus();

  // We use a layout so that the line edit fills the entire cell.
  // Somehow, this does not work with konqueror, but it does respond
  // properly to width, height being set to 100% !
  WApplication *app = WApplication::instance();
  if (app->environment().agent() != WEnvironment::Konqueror) {
    result->setLayout(new WHBoxLayout());
    result->layout()->setContentsMargins(1, 1, 1, 1);
    result->layout()->addWidget(lineEdit);
  } else {
    lineEdit->resize(WLength(100, WLength::Percentage),
		     WLength(100, WLength::Percentage));
    result->addWidget(lineEdit);
  }

  return result;
}

void WItemDelegate::doCloseEditor(WWidget *editor, bool save) const
{
  closeEditor().emit(editor, save);
}

boost::any WItemDelegate::editState(WWidget *editor) const
{
  WContainerWidget *w = dynamic_cast<WContainerWidget *>(editor);
  WLineEdit *lineEdit = dynamic_cast<WLineEdit *>(w->widget(0));

  return boost::any(lineEdit->text());
}

void WItemDelegate::setEditState(WWidget *editor, const boost::any& value) const
{
  WContainerWidget *w = dynamic_cast<WContainerWidget *>(editor);
  WLineEdit *lineEdit = dynamic_cast<WLineEdit *>(w->widget(0));

  lineEdit->setText(boost::any_cast<WT_USTRING>(value));
}

void WItemDelegate::setModelData(const boost::any& editState,
				 WAbstractItemModel *model,
				 const WModelIndex& index) const
{
  model->setData(index, editState, EditRole);
}

}
