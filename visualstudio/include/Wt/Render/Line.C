/*
 * Copyright (C) 2010 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Block.h"
#include "Line.h"
#include "Utils.h"

namespace Wt {
  namespace Render {

const double Line::LEFT_MARGIN_X = -1;

Line::Line(double x, double y, int page)
  : page_(page),
    x_(x),
    y_(y),
    height_(0),
    baseline_(0),
    lineBreak_(false)
{ }

void Line::newLine(double x, double y, int page)
{
  page_ = page;
  x_ = x;
  y_ = y;
  height_ = baseline_ = 0;
  lineBreak_ = false;
}

void Line::addBlock(Block *block)
{
  if (blocks_.empty() || blocks_.back() != block)
    blocks_.push_back(block);
}

void Line::reflow(Block *lineFloat)
{
  if (!lineFloat->blockLayout.empty()) {
    BlockBox& bb = lineFloat->blockLayout[0];

    if (bb.y == y_ && bb.page == page_ && bb.x <= x_)
      x_ += bb.width;
  }
}

void Line::moveToNextPage(BlockList& floats, double minX, double maxX,
			  const WTextRenderer& renderer)
{
  for (unsigned i = 0; i < blocks_.size(); ++i) {
    Block *b = blocks_[i];

    if (b->isFloat())
      Utils::erase(floats, b);
  }

  Block::clearFloats(floats, page_);

  double oldY = y_;
  y_ = 0;
  x_ = minX;
  ++page_;

  BlockList blocks = blocks_;
  blocks_.clear();

  Block::adjustAvailableWidth(y_, page_, x_, maxX, floats);

  for (unsigned i = 0; i < blocks.size(); ++i) {
    Block *b = blocks[i];

    if (b->isFloat()) {
      b->layoutFloat(y_, page_, floats, x_, height_, minX, maxX,
		     false, renderer);
      reflow(b);
    } else {
      for (unsigned j = 0; j < b->inlineLayout.size(); ++j) {
	InlineBox& ib = b->inlineLayout[j];

	if (ib.y == oldY && ib.page == page_ - 1) {
	  if (ib.x != LEFT_MARGIN_X) {
	    ib.x = x_;
	    x_ += ib.width;
	  }

	  ib.page = page_;
	  ib.y = y_;
	}
      }
    }

    blocks_.push_back(b);
  }
}

void Line::adjustHeight(double height, double baseline, double minLineHeight)
{
  if (height_ == 0) {
    height_ = height;
    baseline_ = baseline;
  } else {
    double ascent = std::max(baseline_, baseline);
    double descent = std::max(height_ - baseline_, height - baseline);

    baseline_ = ascent;
    height_ = baseline_ + descent;
  }

  if (minLineHeight > height_) {
    height_ = minLineHeight;
    baseline_ += (minLineHeight - height_)/2.0;
  }
}

void Line::finish(AlignmentFlag textAlign,
		  BlockList& floats, double minX, double maxX,
		  const WTextRenderer& renderer)
{
  /*
   * First right-trim right most text.
   */
  for (unsigned i = 0; i < blocks_.size(); ++i) {
    Block *b = blocks_[blocks_.size() - 1 - i];

    if (!b->isFloat()) {
      if (b->type() != DomElement_LI && b->isText()) {
	bool done = false;

	for (unsigned j = 0; j < b->inlineLayout.size(); ++j) {
	  InlineBox& ib = b->inlineLayout[b->inlineLayout.size() - 1 - j];

	  if (ib.utf8Count > 0) {
	    char lastChar = b->text()[ib.utf8Pos + ib.utf8Count - 1];
	    if (Block::isWhitespace(lastChar)) {
	      --ib.utf8Count;
	      ib.width -= ib.whitespaceWidth;
	    }

	    done = true;
	    break;
	  }
	}

	if (done)
	  break;
      } else
	break;
    }
  }

  double startX = minX, endX = maxX;
  Block::adjustAvailableWidth(y_, page_, startX, endX, floats);
  
  /* Compute total width and total white space width */
  double whitespace = 0;
  double content = 0;

  std::vector<InlineBox *> boxes;

  for (unsigned i = 0; i < blocks_.size(); ++i) {
    Block *b = blocks_[i];

    if (b->isFloat())
      b->layoutFloat(y_ + height_, page_, floats,
		     minX, 0, minX, maxX, false, renderer);
    else {
      for (unsigned j = 0; j < b->inlineLayout.size(); ++j) {
	InlineBox& ib = b->inlineLayout[j];

	if (ib.y == y_ && ib.page == page_) {
	  std::string va = b->cssProperty(PropertyStyleVerticalAlign);

	  if (va == "top")
	    ib.y = y_;
	  else if (va == "bottom")
	    ib.y = y_ + height_ - ib.height;
	  else
	    ib.y += baseline_ - ib.baseline;

	  if (ib.x != LEFT_MARGIN_X) {
	    boxes.push_back(&ib);

	    content += ib.width;
	    ib.whitespaceCount = 0;

	    if (b->isText()) {
	      for (int i = 0; i < ib.utf8Count; ++i) {
		if (Block::isWhitespace(b->text()[ib.utf8Pos + i]))
		  ++ib.whitespaceCount;
	      }

	      content -= ib.whitespaceWidth * ib.whitespaceCount;
	      whitespace += ib.whitespaceWidth * ib.whitespaceCount;
	    }
	  } else {
	    /* Positioned in the margin */
	    ib.x = startX - ib.width;
	  }
	}
      }
    }
  }

  double spaceFactor = 1.0;

  switch (textAlign) {
  case AlignLeft:
    break;
  case AlignRight:
    startX = endX - content - whitespace;
    break;
  case AlignCenter:
    startX += (endX - startX - content - whitespace)/2;
    break;
  case AlignJustify:
    if (!lineBreak_) {
      double remaining = endX - startX - content;

      if (whitespace > 0)
	spaceFactor = remaining / whitespace;
    }
    break;
  default:
    throw std::runtime_error("Unsupported text-align attribute");
  }

  double x = startX;

  for (unsigned i = 0; i < boxes.size(); ++i) {
    InlineBox& ib = *boxes[i];

    ib.x = x;

    double contentWidth = ib.width - ib.whitespaceWidth * ib.whitespaceCount;
    ib.whitespaceWidth *= spaceFactor;

    ib.width = contentWidth + ib.whitespaceWidth * ib.whitespaceCount;
    x += ib.width;
  }

  blocks_.clear();
}

  }
}
