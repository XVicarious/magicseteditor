//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2007 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <gui/value/symbol.hpp>
#include <gui/symbol/window.hpp>

// ----------------------------------------------------------------------------- : SymbolValueEditor

IMPLEMENT_VALUE_EDITOR(Symbol)
	, button_down(-2)
{}

void SymbolValueEditor::draw(RotatedDC& dc) {
	SymbolValueViewer::draw(dc);
	// draw helper text if there are no symbols
	if (symbols.empty()) {
		dc.SetFont(wxFont(10,wxSWISS,wxNORMAL,wxNORMAL));
		dc.SetTextForeground(*wxBLACK);
		RealSize text_size = dc.GetTextExtent(_("double click to edit symbol"));
		dc.DrawText(_("double click to edit symbol"), align_in_rect(ALIGN_MIDDLE_CENTER, text_size, style().getRect()));
	}
	if (nativeLook()) {
		// draw editor buttons
		dc.SetFont(*wxNORMAL_FONT);
		drawButton(dc, 0, _("Edit"));
		drawButton(dc, 1, _("Gallery"));
	}
}
void SymbolValueEditor::drawButton(RotatedDC& dc, int button, const String& text) {
	bool down = button == button_down;
	double size = style().height;
	double x = style().right - size - (size + 1) * button;
	double y = style().top;
	// draw button
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	dc.DrawRectangle(RealRect(x, y, size, size));
	dc.SetPen(wxSystemSettings::GetColour(down ? wxSYS_COLOUR_BTNSHADOW : wxSYS_COLOUR_BTNHIGHLIGHT));
	dc.DrawLine(RealPoint(x,y),RealPoint(x+size,y));
	dc.DrawLine(RealPoint(x,y),RealPoint(x,y+size));
	dc.SetPen(wxSystemSettings::GetColour(down ? wxSYS_COLOUR_BTNHIGHLIGHT : wxSYS_COLOUR_BTNSHADOW));
	dc.DrawLine(RealPoint(x+size-1,y),RealPoint(x+size-1,y+size));
	dc.DrawLine(RealPoint(x,y+size-1),RealPoint(x+size,y+size-1));
	// draw text
	RealSize text_size = dc.GetTextExtent(text);
	dc.DrawText(text, align_in_rect((Alignment)(ALIGN_BOTTOM | ALIGN_CENTER), text_size, RealRect(x, y, size,size*0.9)));
}

int SymbolValueEditor::findButton(const RealPoint& pos) {
	if (pos.y < style().top || pos.y >= style().bottom) return -1;
	int button = (int)floor( (style().right - pos.x) / (style().height + 1) );
	if (button >= 0 && button <= 1) return button;
	return -1;
}

bool SymbolValueEditor::onLeftDown(const RealPoint& pos, wxMouseEvent&) {
	if (!nativeLook()) return false;
	int button = findButton(pos);
	if (button != button_down) {
		button_down = button;
		viewer.redraw(*this);
	}
	return true;
}
bool SymbolValueEditor::onMotion(const RealPoint& pos, wxMouseEvent& ev) {
	if (button_down != -2) {
		int button = findButton(pos);
		if (button != button_down) {
			button_down = button;
			viewer.redraw(*this);
		}
	}
	return true;
}

bool SymbolValueEditor::onLeftUp(const RealPoint& pos, wxMouseEvent&) {
	if (!nativeLook()) return false;
	if (button_down == 0) {
		// edit
		button_down = -2;
		viewer.redraw(*this);
		SymbolWindow* wnd = new SymbolWindow(nullptr, valueP(), viewer.getSet());
		wnd->Show();
		return true;
	} else if (button_down == 1) {
		// gallery
		button_down = -2;
		viewer.redraw(*this);
		// TODO
		return true;
	} else {
		button_down = -2;
		return false;
	}
}

bool SymbolValueEditor::onLeftDClick(const RealPoint& pos, wxMouseEvent&) {
	// Use SetWindow as parent? Maybe not, the symbol editor will stay open when mainwindow closes
	SymbolWindow* wnd = new SymbolWindow(nullptr, valueP(), viewer.getSet());
	wnd->Show();
	return true;
}

void SymbolValueEditor::determineSize(bool) {
	style().height = 50;
}
