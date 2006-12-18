//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_GUI_CONTROL_SELECT_CARD_LIST
#define HEADER_GUI_CONTROL_SELECT_CARD_LIST

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <gui/control/card_list.hpp>

// ----------------------------------------------------------------------------- : SelectCardList

/// A card list with check boxes
class SelectCardList : public CardListBase {
  public:
	SelectCardList(Window* parent, int id, long additional_style = 0);
	/// Select all cards
	void selectAll();
	/// Deselect all cards
	void selectNone();
	/// Is the given card selected?
	bool isSelected(const CardP& card) const;
  protected:
	int    OnGetItemImage(long pos) const;
  private:
	DECLARE_EVENT_TABLE();
	
	set<CardP> selected; ///< which cards are selected?
	
	void toggle(const CardP& card);
	
	void onKeyDown(wxKeyEvent&);
	void onLeftDown(wxMouseEvent&);
};


// ----------------------------------------------------------------------------- : EOF
#endif
