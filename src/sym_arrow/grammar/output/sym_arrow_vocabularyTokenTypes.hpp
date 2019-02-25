#ifndef INC_sym_arrow_vocabularyTokenTypes_hpp_
#define INC_sym_arrow_vocabularyTokenTypes_hpp_

/* $ANTLR 2.7.7 (2006-11-01): "sym_arrow.g" -> "sym_arrow_vocabularyTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API sym_arrow_vocabularyTokenTypes {
#endif
	enum {
		EOF_ = 1,
		PLUS = 4,
		MINUS = 5,
		MULT = 6,
		DIV = 7,
		POWER = 8,
		OR = 9,
		LPAREN = 10,
		RPAREN = 11,
		LANGLE = 12,
		COMMA = 13,
		RANGLE = 14,
		ID = 15,
		LBRACK = 16,
		RBRACK = 17,
		NUMBER = 18,
		INT = 19,
		LCURL = 20,
		RCURL = 21,
		SEMI = 22,
		ESC = 23,
		WHITESPACE = 24,
		NEWLINE = 25,
		NOT_NEWLINE = 26,
		NON_NEWLINE_WHITESPACE = 27,
		COMMENT = 28,
		SL_COMMENT = 29,
		SL_DCOMMENT = 30,
		ML_COMMENT = 31,
		STRING_LITERAL = 32,
		MULTILINE_STRING = 33,
		CONTINUATION = 34,
		DIGIT = 35,
		EXPONENT = 36,
		COMPLEX = 37,
		DDOT = 38,
		DOT = 39,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_sym_arrow_vocabularyTokenTypes_hpp_*/
