//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_UTIL_TAGGED_STRING
#define HEADER_UTIL_TAGGED_STRING

/** @file util/tagged_string.hpp
 *
 *  @brief Utility for working with strings with tags
 */
 
// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>

// ----------------------------------------------------------------------------- : Conversion to/from normal string

/// Remove all tags from a string and convert escaped '<' back to normal '<'
/** e.g. "<sym>R</> something <i>(note)</>"
 *  becomes "R something (note)"
 */
String untag(const String&);

/// Remove all tags from a string, but doesn't convert back escape codes
/** untag_no_escape(x) = escape(untag(x)) */
String untag_no_escape(const String&);

/// Remove all tags from a string
/** In addition to what untag() does this function also removes <sep-soft>...</sep-soft>
 */
String untag_hide_sep(const String&);

/// Escapes a String by converting '>' to '\1'
String escape(const String&);

/// Convert old style </> close tags to new style </tag> tags
/** This style was used until 0.2.6 */
String fix_old_tags(const String&);

// ----------------------------------------------------------------------------- : Finding tags

/// Returns the position of the tag the given position is in
/** Returns String::npos if pos is not in a tag.
 *  In a tag is:
 *   < t a g >
 *  n y y y y n
 */
size_t tag_start(const String& str, size_t pos);

/// Returns the position just beyond the tag starting at start
size_t skip_tag(const String& str, size_t start);

/// Find the position of the closing tag matching the tag at start
/** If not found returns String::npos */
size_t match_close_tag(const String& str, size_t start);

/// Find the last start tag before position start
/** If not found returns String::npos */
size_t last_start_tag_before(const String& str, const String& tag, size_t start);

/// Is the given range entirely contained in a given tagged block?
/** If so: return the start position of that tag, otherwise returns String::npos
 *  A tagged block is everything between <tag>...</tag>
 */
size_t in_tag(const String& str, const String& tag, size_t start, size_t end);

/// Return the tag at the given position (without the <>)
String tag_at(const String& str, size_t pos);

/// Return the tag at the given position (without the <>)
/** stops at '-', so for "<kw-a>" returns "kw" */
String tag_type_at(const String& str, size_t pos);

/// Matching close tag for a tag
String close_tag(const String& tag);

/// The matching close tag for an open tag and vice versa
String anti_tag(const String& tag);

// ----------------------------------------------------------------------------- : Cursor position

/// Directions of cursor movement
enum Movement
{	MOVE_LEFT  = -1	///< Always move the cursor to the left
,	MOVE_MID   =  0	///< Move in whichever direction the distance to move is shorter (TODO: define shorter)
,	MOVE_RIGHT =  1	///< Always move the cursor to the right
};

/// Find the cursor position corresponding to the given character index.
/** A cursor position always corresponds to a valid place to type text.
 *  The cursor position is rounded to the direction dir.
 */
size_t index_to_cursor(const String& str, size_t index, Movement dir = MOVE_MID);

/// Find the range of character indeces corresponding to the given cursor position
/** The output parameters will correspond to the range [start...end) which are all valid character indices.
 */
void cursor_to_index_range(const String& str, size_t cursor, size_t& begin, size_t& end);

/// Find the character index corresponding to the given cursor position
size_t cursor_to_index(const String& str, size_t cursor, Movement dir = MOVE_MID);

// ----------------------------------------------------------------------------- : Global operations

/// Remove all instances of a tag and its close tag, but keep the contents.
/** tag doesn't have to be a complete tag, for example remove_tag(str, "<kw-")
 *  removes all tags starting with "<kw-".
 */
String remove_tag(const String& str, const String& tag);

/// Remove all instances of tags starting with tag, but not its close tag
String remove_tag_exact(const String& str, const String& tag);

/// Remove all instances of a tag (including contents) from a string
/** tag doesn't have to be a complete tag, for example remove_tag_contents(str, "<kw-")
 *  removes all tags starting with "<kw-".
 */
String remove_tag_contents(const String& str, const String& tag);

// ----------------------------------------------------------------------------- : Updates

/// Replace a subsection of 'input' with 'replacement'.
/** The section to replace is indicated by [start...end).
 *  This function makes sure tags still match. It also attempts to cancel out tags.
 *  This means that when removing "<x>a</x>" nothing is left,
 *  but with input "<x>a" -> "<x>" and "</>a" -> "</>".
 *  Escapes the replacement, i.e. all < in become \1.
 */
String tagged_substr_replace(const String& input, size_t start, size_t end, const String& replacement);

// ----------------------------------------------------------------------------- : Simplification

/// Verify that a string is correctly tagged, if it is not, change it so it is
/** Ensures that:
 *   - All tags end, i.e. for each '<' there is a matching '>'
 *   - There are no tags containing '<'
 */
String verify_tagged(const String& str);

/// Simplify a tagged string
/**   - merges adjecent open/close tags: "<tag></tag>" --> ""
 *    - removes overlapping tags: "<i>a<i>b</i>c</i>" --> "<i>abc</i>"
 */
String simplify_tagged(const String& str);

/// Simplify a tagged string by merging adjecent open/close tags
/** e.g. "<tag></tag>" --> ""
 */
String simplify_tagged_merge(const String& str);

/// Simplify overlapping formatting tags
/** e.g. "<i>blah<i>blah</i>blah</i>" -> "<i>blahblahblah</i>"
 */
String simplify_tagged_overlap(const String& str);

// ----------------------------------------------------------------------------- : EOF
#endif
