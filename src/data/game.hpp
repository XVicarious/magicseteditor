//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_DATA_GAME
#define HEADER_DATA_GAME

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <util/io/package.hpp>
#include <script/scriptable.hpp>
#include <script/dependency.hpp>
#include <util/dynamic_arg.hpp>

DECLARE_POINTER_TYPE(Field);
DECLARE_POINTER_TYPE(Game);
DECLARE_POINTER_TYPE(StatsDimension);
DECLARE_POINTER_TYPE(StatsCategory);
DECLARE_POINTER_TYPE(KeywordParam);
DECLARE_POINTER_TYPE(KeywordMode);
DECLARE_POINTER_TYPE(Keyword);

// ----------------------------------------------------------------------------- : Game

/// Game that is used for cards constructed with the default constructor, as well as for reading stylesheets
DECLARE_DYNAMIC_ARG(Game*, game_for_reading);

/// A description of a card game
class Game : public Packaged {
  public:
	Game();
	
	OptionalScript init_script;		///< Script of variables available to other scripts in this game
	vector<FieldP> style_fields;		///< Style-specific boolean data
	vector<FieldP> set_fields;		///< Fields for set information
	vector<FieldP> card_fields;		///< Fields on each card
	vector<StatsDimensionP> statistics_dimensions;	///< (Additional) statistics dimensions
	vector<StatsCategoryP>  statistics_categories;	///< (Additional) statistics categories
	
	vector<KeywordParamP>   keyword_parameter_types;///< Types of keyword parameters
	vector<KeywordModeP>    keyword_modes;          ///< Modes of keywords
	vector<KeywordP>        keywords;		///< Keywords for use in text
	
	Dependencies dependent_scripts_cards;		///< scripts that depend on the card list
	Dependencies dependent_scripts_keywords;	///< scripts that depend on the keywords
	bool dependencies_initialized;			///< are the script dependencies comming from this game all initialized?
	
	/// Loads the game with a particular name, for example "magic"
	static GameP byName(const String& name);
	
	/// Is this Magic the Gathering?
	bool isMagic() const;
	
	static String typeNameStatic();
	virtual String typeName() const;
	
  protected:
	virtual void validate(Version);
	
	DECLARE_REFLECTION();
};

// ----------------------------------------------------------------------------- : EOF
#endif
