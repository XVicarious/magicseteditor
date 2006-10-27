//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <data/card.hpp>
#include <data/game.hpp>
#include <data/field.hpp>
#include <util/error.hpp>
#include <script/value.hpp>

DECLARE_TYPEOF_COLLECTION(FieldP);

// ----------------------------------------------------------------------------- : Card

Card::Card() {
	if (!game_for_reading()) {
		throw InternalError(_("game_for_reading not set"));
	}
	data.init(game_for_reading()->card_fields);
}

Card::Card(const Game& game) {
	data.init(game.card_fields);
}

String Card::identification() const {
	return _("TODO");
}

IMPLEMENT_REFLECTION(Card) {
	REFLECT(stylesheet);
	REFLECT(notes);
	REFLECT_NAMELESS(data);
}

