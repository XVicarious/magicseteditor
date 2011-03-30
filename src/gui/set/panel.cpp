//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2010 Twan van Laarhoven and Sean Hunt             |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <gui/set/panel.hpp>
#include <data/card.hpp>

// ----------------------------------------------------------------------------- : SetWindowPanel

SetWindowPanel::SetWindowPanel(Window* parent, int id, const SetP& set, bool autoTabbing)
	: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, autoTabbing ? wxTAB_TRAVERSAL : 0)
	, SetView(set)
{}

CardP_nullable SetWindowPanel::selectedCard() const {
	return CardP_nullable();
}

bool SetWindowPanel::isInitialized() const {
	return !GetChildren().IsEmpty();
}
