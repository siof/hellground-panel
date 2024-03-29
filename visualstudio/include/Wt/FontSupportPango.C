/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/WApplication"
#include "Wt/WFont"
#include "Wt/WFontMetrics"
#include "Wt/WPointF"
#include "Wt/WRectF"
#include "Wt/WTransform"
#include "Wt/FontSupport.h"

#include "Utils.h"

#ifdef WT_THREADED
#include <boost/thread.hpp>
#endif // WT_THREADED

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <pango/pango.h>
#include <pango/pangoft2.h>

#ifndef DOXYGEN_ONLY

namespace {

double pangoUnitsToDouble(const int u) 
{
  return u / PANGO_SCALE;
}

int pangoUnitsFromDouble(const double u)
{
  return (int) u * PANGO_SCALE;
}

/*
 * A global font map, since this one leaks as hell, and it cannot stand
 * being used in thread local storage since cleanup (with thread exit)
 * doesn't work properly.
 *
 * It is not clear what operations involving the font map are thread-safe,
 * so here we serialize everything using a mutex.
 */

PangoFontMap *pangoFontMap = 0;

#ifdef WT_THREADED
boost::recursive_mutex pangoMutex;
#define PANGO_LOCK boost::recursive_mutex::scoped_lock lock(pangoMutex);
#else
#define PANGO_LOCK
#endif // WT_THREADED
}

namespace Wt {

FontSupport::Bitmap::Bitmap(int width, int height)
  : width_(width),
    height_(height),
    pitch_((width_ + 3) & -4)
{
  buffer_ = new unsigned char[height_ * pitch_];
  memset(buffer_, 0, height_ * pitch_);
}

FontSupport::Bitmap::~Bitmap()
{
  delete[] buffer_;
}

FontSupport::FontMatch::FontMatch(PangoFont *font)
  : font_(font)
{ }

std::string FontSupport::FontMatch::fileName() const
{
  return FontSupport::fontPath(font_);
}

FontSupport::FontSupport(WPaintDevice *paintDevice)
  : device_(paintDevice),
    matchFont_(0)
{
  PANGO_LOCK;

  if (!pangoFontMap)
    pangoFontMap = pango_ft2_font_map_new();

  context_
    = pango_ft2_font_map_create_context((PangoFT2FontMap*)pangoFontMap);

  currentFont_ = 0;
}

FontSupport::~FontSupport()
{
  PANGO_LOCK;

  if (matchFont_)
    g_object_unref(matchFont_);

  g_object_unref(context_);
}

bool FontSupport::canRender() const
{
  return true;
}

void FontSupport::setDevice(WPaintDevice *paintDevice)
{
  device_ = paintDevice;
}

PangoFontDescription *FontSupport::createFontDescription(const WFont& f) const
{
  std::string s = f.specificFamilies().toUTF8();

  /* Quick fix for Mac OS X (Times is a special font?) */
  boost::trim(s);
  if (Utils::lowerCase(s) == "times")
    s = "Times New Roman";

  if (f.genericFamily() != WFont::Default) {
    if (!s.empty())
      s += ',';

    switch (f.genericFamily()) {
    case WFont::Serif: s += "serif"; break;
    case WFont::SansSerif: s += "sans"; break;
    case WFont::Cursive: s += "cursive"; break;
    case WFont::Fantasy: s += "fantasy"; break;
    case WFont::Monospace: s += "monospace"; break;
    default: break;
    }
  }

  if (f.weightValue() < 300)
    s += " ultra-light";
  else if (f.weightValue() < 400)
    s += " light";
  else if (f.weightValue() >= 700)
    s += " bold";
  else if (f.weightValue() >= 800)
    s += " ultra-bold";
  else if (f.weightValue() >= 900)
    s += " heavy";

  switch (f.style()) {
  case WFont::Italic: s += " italic"; break;
  case WFont::Oblique: s += " oblique"; break;
  default: break;
  }

  switch (f.variant()) {
  case WFont::SmallCaps: s += " small-caps"; break;
  default: break;
  }

  s += " " + boost::lexical_cast<std::string>((int)(f.sizeLength().toPixels()
						    * 0.75 * 96/72));

  return pango_font_description_from_string(s.c_str());
}

FontSupport::FontMatch FontSupport::matchFont(const WFont& f) const
{
  PANGO_LOCK;

  PangoFontDescription *desc = createFontDescription(f);

  if (matchFont_)
    g_object_unref(matchFont_);
  matchFont_ = pango_font_map_load_font(pangoFontMap, context_, desc);

  pango_font_description_free(desc);

  return FontMatch(matchFont_);
}

void FontSupport::addFontCollection(const std::string& directory,
				      bool recursive)
{
}

bool FontSupport::busy() const
{
  return currentFont_;
}

std::string FontSupport::drawingFontPath() const
{
  return fontPath(currentFont_);
}

std::string FontSupport::fontPath(PangoFont *font)
{
  PANGO_LOCK;

  PangoFcFont *f = (PangoFcFont *)font;
  FT_Face face = pango_fc_font_lock_face(f);

  std::string result;
  if (face->stream->pathname.pointer)
    result = (const char *)face->stream->pathname.pointer;

  pango_fc_font_unlock_face(f);

  return result;
}

GList *FontSupport::layoutText(const WFont& font,
			       const std::string& utf8,
			       std::vector<PangoGlyphString *>& glyphs,
			       int& width)
{
  PANGO_LOCK;

  PangoFontDescription *desc = createFontDescription(font);
  pango_context_set_font_description(context_, desc);
  pango_font_description_free(desc);

  PangoAttrList *attrs = pango_attr_list_new();

  GList *items
    = pango_itemize(context_, utf8.c_str(), 0, utf8.length(), attrs, NULL);

  width = 0;

  for (GList *elem = items; elem; elem = elem->next) {
    PangoItem *item = (PangoItem *)elem->data;
    PangoAnalysis *analysis = &item->analysis;

    PangoGlyphString *gl = pango_glyph_string_new();

    pango_shape(utf8.c_str() + item->offset, item->length, analysis, gl);

    glyphs.push_back(gl);

    if (device_) {
      currentFont_ = analysis->font;

      WTextItem textItem
	= device_->measureText(WString::fromUTF8(utf8.substr(item->offset,
							     item->length)),
			       -1, false);

      width += pangoUnitsFromDouble(textItem.width());

      currentFont_ = 0;
    } else
      width += pango_glyph_string_get_width(gl);
  }

  pango_attr_list_unref(attrs);

  return items;
}

void FontSupport::drawText(const WFont& font, const WRectF& rect,
			   WFlags<AlignmentFlag> flags, const WString& text)
{
  PANGO_LOCK;

  std::string utf8 = text.toUTF8();

  std::vector<PangoGlyphString *> glyphs;
  int width;

  GList *items = layoutText(font, utf8, glyphs, width);

  AlignmentFlag hAlign = flags & AlignHorizontalMask;
  AlignmentFlag vAlign = flags & AlignVerticalMask;

  /* FIXME handle bidi ! */

  double x;
  switch (hAlign) {
  case AlignLeft:
    x = rect.left();
    break;
  case AlignRight:
    x = rect.right() - pangoUnitsToDouble(width);
    break;
  case AlignCenter:
    x = rect.center().x() - pangoUnitsToDouble(width/2);
    break;
  default:
    x = 0;
  }

  unsigned i = 0;
  for (GList *elem = items; elem; elem = elem->next) {
    PangoItem *item = (PangoItem *)elem->data;
    PangoAnalysis *analysis = &item->analysis;

    PangoGlyphString *gl = glyphs[i++];

    currentFont_ = analysis->font;

    /*
     * Note, we are actually ignoring the selected glyphs here, which
     * is a pitty and possibly wrong if the device does not make the
     * same selection !
     */
    WString s = WString::fromUTF8(utf8.substr(item->offset,
					      item->length));

    device_->drawText(WRectF(x, rect.y(),
			     1000, rect.height()),
		      AlignLeft | vAlign, TextSingleLine,
		      s);

    WTextItem textItem = device_->measureText(s, -1, false);

    x += textItem.width();

    pango_item_free(item);
    pango_glyph_string_free(gl);
  }

  g_list_free(items);

  currentFont_ = 0;
}

WFontMetrics FontSupport::fontMetrics(const WFont& font)
{
  PANGO_LOCK;

  PangoFont *pangoFont = matchFont(font).pangoFont();
  PangoFontMetrics *metrics = pango_font_get_metrics(pangoFont, NULL);

  double ascent
    = pangoUnitsToDouble(pango_font_metrics_get_ascent(metrics));
  double descent 
    = pangoUnitsToDouble(pango_font_metrics_get_descent(metrics));

  double leading = (ascent + descent) - font.sizeLength(12).toPixels();
  ascent -= leading;

  WFontMetrics result(font, leading, ascent, descent);

  pango_font_metrics_unref(metrics);

  return result;
}

WTextItem FontSupport::measureText(const WFont& font, const WString& text,
				   double maxWidth, bool wordWrap)
{
  PANGO_LOCK;

  /*
   * Note: accurate measuring on a bitmap requires that the transformation
   * is applied, because hinting may push chars to boundaries e.g. when
   * rotated (or scaled too?)
   */
  std::string utf8 = text.toUTF8();
  const char *s = utf8.c_str();

  if (wordWrap) {
    int utflen = g_utf8_strlen(s, -1);
    PangoLogAttr *attrs = new PangoLogAttr[utflen + 1];
    PangoLanguage *language = pango_language_from_string("en-US");

    pango_get_log_attrs(s, utf8.length(), -1, language, attrs, utflen + 1);

    double w = 0, nextW = -1;

    int current = 0;
    int measured = 0;
    int end = 0;

    bool maxWidthReached = false;

    for (int i = 0; i < utflen + 1; ++i) {
      if (i == utflen || attrs[i].is_line_break) {
	int cend = g_utf8_offset_to_pointer(s, end) - s;

	WTextItem ti
	  = measureText(font, WString::fromUTF8(utf8.substr(measured,
							    cend - measured)),
			-1, false);

	if (w + ti.width() > maxWidth) {
	  nextW = ti.width();
	  maxWidthReached = true;
	  break;
	} else {
	  measured = cend;
	  current = g_utf8_offset_to_pointer(s, i) - s;
	  w += ti.width();

	  if (i == utflen)
	    w += measureText(font, WString::fromUTF8(utf8.substr(measured)),
			     -1, false).width();
	}
      }

      if (!attrs[i].is_white)
	end = i + 1;
    }

    delete[] attrs;

    if (maxWidthReached) {
      return WTextItem(WString::fromUTF8(utf8.substr(0, current)), w, nextW);
    } else {
      return WTextItem(text, w);
    }
  } else {
    std::vector<PangoGlyphString *> glyphs;
    int width;

    GList *items = layoutText(font, utf8, glyphs, width);

    double w = pangoUnitsToDouble(width);

    for (unsigned i = 0; i < glyphs.size(); ++i)
      pango_glyph_string_free(glyphs[i]);

    g_list_foreach(items, (GFunc) pango_item_free, 0);
    g_list_free(items);

    return WTextItem(text, w);
  }
}

void FontSupport::drawText(const WFont& font, const WRectF& rect,
			   const WTransform& transform, Bitmap& bitmap,
			   WFlags<AlignmentFlag> flags,
			   const WString& text)
{
  PANGO_LOCK;

  PangoMatrix matrix;
  matrix.xx = transform.m11();
  matrix.xy = transform.m21();
  matrix.yx = transform.m12();
  matrix.yy = transform.m22();
  matrix.x0 = transform.dx();
  matrix.y0 = transform.dy();

  std::string utf8 = text.toUTF8();

  std::vector<PangoGlyphString *> glyphs;
  int width;

  pango_context_set_matrix(context_, &matrix);

  /*
   * Oh my god, somebody explain me why we need to do this...
   */
  WFont f = font;
  f.setSize(font.sizeLength().toPixels()
	    / pango_matrix_get_font_scale_factor(&matrix));

  GList *items = layoutText(f, utf8, glyphs, width);
  pango_context_set_matrix(context_, 0);

  AlignmentFlag hAlign = flags & AlignHorizontalMask;

  /* FIXME handle bidi ! */

  double x;
  switch (hAlign) {
  case AlignLeft:
    x = rect.left();
    break;
  case AlignRight:
    x = rect.right() - pangoUnitsToDouble(width);
    break;
  case AlignCenter:
    x = rect.center().x() - pangoUnitsToDouble(width/2);
    break;
  default:
    x = 0;
  }

  AlignmentFlag vAlign = flags & AlignVerticalMask;

  PangoFont *pangoFont = matchFont(font).pangoFont();
  PangoFontMetrics *metrics = pango_font_get_metrics(pangoFont, NULL);

  double ascent
    = pangoUnitsToDouble(pango_font_metrics_get_ascent(metrics));
  double descent 
    = pangoUnitsToDouble(pango_font_metrics_get_descent(metrics));

  pango_font_metrics_unref(metrics);

  double baseline = ascent;
  double height = ascent + descent;

  double y;
  switch (vAlign) {
  case AlignTop:
    y = rect.top() + baseline;
    break;
  case AlignMiddle:
    y = rect.center().y() - height / 2 + baseline;
    break;
  case AlignBottom:
    y = rect.bottom() - height + baseline;
    break;
  default:
    y = 0;
  }

  FT_Bitmap bmp;
  bmp.buffer = bitmap.buffer();
  bmp.width = bitmap.width();
  bmp.rows = bitmap.height();
  bmp.pitch = bitmap.pitch();
  bmp.pixel_mode = FT_PIXEL_MODE_GRAY;
  bmp.num_grays = 16; // ???

  GList *elem;
  unsigned i = 0;

  for (elem = items; elem; elem = elem->next) {
    PangoItem *item = (PangoItem *)elem->data;
    PangoAnalysis *analysis = &item->analysis;

    PangoGlyphString *gl = glyphs[i++];

    pango_ft2_render_transformed(&bmp, &matrix,
				 analysis->font, gl,
				 pangoUnitsFromDouble(x),
				 pangoUnitsFromDouble(y));

    x += pangoUnitsToDouble(pango_glyph_string_get_width(gl));

    pango_glyph_string_free(gl);
    pango_item_free(item);
  }

  g_list_free(items);
}

}

#endif // DOXYGEN_ONLY
