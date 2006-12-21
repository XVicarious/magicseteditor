//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_SCRIPT_DEPENDENCY
#define HEADER_SCRIPT_DEPENDENCY

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>

// ----------------------------------------------------------------------------- : Dependency

/// Types of dependencies
enum DependencyType
{	DEP_CARD_FIELD			///< dependency of a script in a "card" field
,	DEP_CARDS_FIELD			///< dependency of a script in a "card"  field for all cards
,	DEP_SET_FIELD			///< dependency of a script in a "set"   field
,	DEP_STYLE				///< dependency of a script in a "style" property, data gives the stylesheet
,	DEP_CARD_COPY_DEP		///< copy the dependencies from a card field
,	DEP_SET_COPY_DEP		///< copy the dependencies from a set  field
};

/// A 'pointer' to some script that depends on another script
class Dependency {
  public:
	inline Dependency(DependencyType type, size_t index, void* data = nullptr)
		: type(type), index(index), data(data)
	{}
	
	DependencyType type   : 5;	///< Type of the dependent script
	size_t         index  : 27;	///< index into an IndexMap
	void*          data;		///< Extra pointer data
	
	/// This dependency, but dependent on all cards instead of just one
	inline Dependency makeCardIndependend() const {
		return Dependency(type == DEP_CARD_FIELD ? DEP_CARDS_FIELD : type, index, data);
	}
};

// ----------------------------------------------------------------------------- : EOF
#endif
