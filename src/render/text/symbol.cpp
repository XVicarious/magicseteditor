//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <render/text/element.hpp>
#include <data/symbol_font.hpp>

// ----------------------------------------------------------------------------- : SymbolTextElement

void SymbolTextElement::draw(RotatedDC& dc, double scale, const RealRect& rect, const double* xs, DrawWhat what, size_t start, size_t end) const {
	if (font.font) {
		font.font->draw(dc, ctx, rect, font.size * scale, font.alignment, text.substr(start, end-start));
	}
}

void SymbolTextElement::getCharInfo(RotatedDC& dc, double scale, vector<CharInfo>& out) const {
	if (font.font) {
		font.font->getCharInfo(dc, ctx, font.size * scale, text.substr(start, end-start), out);
	}
}

double SymbolTextElement::minScale() const {
	return min(font.size, font.scale_down_to) / max(0.01, font.size);
}
double SymbolTextElement::scaleStep() const {
	return 1. / max(font.size, 1.);
}
