/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/WFontMetrics"

namespace Wt {

WFontMetrics::WFontMetrics(const WFont& font, double leading, double ascent,
			   double descent)
  : font_(font),
    leading_(leading),
    ascent_(ascent),
    descent_(descent)
{ }

double WFontMetrics::size() const
{
  return font_.sizeLength(12).toPixels();
}

}
