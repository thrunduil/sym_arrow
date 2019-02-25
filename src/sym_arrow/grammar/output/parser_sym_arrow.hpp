#ifndef INC_parser_sym_arrow_hpp_
#define INC_parser_sym_arrow_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "sym_arrow.g" -> "parser_sym_arrow.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "sym_arrow_vocabularyTokenTypes.hpp"
#include <antlr/LLkParser.hpp>


    //parser.hpp includes
    #include "grammar/parser_include.h"

class CUSTOM_API parser_sym_arrow : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public sym_arrow_vocabularyTokenTypes
{
    
    //parser members
    #include "grammar/parser_members.h"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	parser_sym_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	parser_sym_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	parser_sym_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	parser_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	parser_sym_arrow(antlr::pool_parser pool, const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return parser_sym_arrow::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return parser_sym_arrow::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return parser_sym_arrow::tokenNames;
	}
	public: expr  stringUnit();
	public: expr  term();
	public: expr  addExpr();
	public: expr  multExpr();
	public: expr  sgnAtom();
	public: expr  powExpr();
	public: expr  primaryExpr();
	public: expr  atomExpr();
	public: expr  postfixExpr();
	public: expr  atoms();
	public: expr  final_values();
	public: expr  star_or_symbol();
	public: symbol  scoped_symbol();
	public: symbol  atom_symbol();
	public: expr  atom_number();
	public: int  atom_int();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 40;
#else
	enum {
		NUM_TOKENS = 40
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
};

#endif /*INC_parser_sym_arrow_hpp_*/
