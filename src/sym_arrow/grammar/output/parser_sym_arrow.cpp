/* $ANTLR 2.7.7 (2006-11-01): "sym_arrow.g" -> "parser_sym_arrow.cpp"$ */
#include "parser_sym_arrow.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>

//parser.cpp includes

parser_sym_arrow::parser_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,tokenBuf,k)
{
}

parser_sym_arrow::parser_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,tokenBuf,1)
{
}

parser_sym_arrow::parser_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,lexer,k)
{
}

parser_sym_arrow::parser_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,lexer,1)
{
}

parser_sym_arrow::parser_sym_arrow(antlr::pool_parser pool,const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,state,1)
{
}

expr  parser_sym_arrow::stringUnit() {
	expr x;
	
	try {      // for error handling
		x=term();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_0);
	}
	return x;
}

expr  parser_sym_arrow::term() {
	expr x;
	
	try {      // for error handling
		x=addExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
	return x;
}

expr  parser_sym_arrow::addExpr() {
	expr x;
	
	expr tmp;
	
	
	try {      // for error handling
		x=multExpr();
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case PLUS:
			{
				match(PLUS);
				tmp=multExpr();
				x = std::move(x) + std::move(tmp);
				break;
			}
			case MINUS:
			{
				match(MINUS);
				tmp=multExpr();
				x = std::move(x) - std::move(tmp);
				break;
			}
			default:
			{
				goto _loop5;
			}
			}
		}
		_loop5:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_1);
	}
	return x;
}

expr  parser_sym_arrow::multExpr() {
	expr x;
	
	expr tmp;
	
	
	try {      // for error handling
		x=sgnAtom();
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case MULT:
			{
				match(MULT);
				tmp=sgnAtom();
				x = std::move(x) * std::move(tmp);
				break;
			}
			case DIV:
			{
				match(DIV);
				tmp=sgnAtom();
				x = std::move(x) / std::move(tmp);
				break;
			}
			default:
			{
				goto _loop8;
			}
			}
		}
		_loop8:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_2);
	}
	return x;
}

expr  parser_sym_arrow::sgnAtom() {
	expr x;
	
	expr tmp;
	
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case MINUS:
		{
			match(MINUS);
			x=powExpr();
			x = - std::move(x);
			break;
		}
		case PLUS:
		{
			match(PLUS);
			x=powExpr();
			break;
		}
		case OR:
		case LPAREN:
		case ID:
		case NUMBER:
		case INT:
		{
			x=powExpr();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::powExpr() {
	expr x;
	
	expr tmp;
	
	
	try {      // for error handling
		x=primaryExpr();
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == POWER)) {
				match(POWER);
				tmp=sgnAtom();
				x = make_power(std::move(x),std::move(tmp));
			}
			else {
				goto _loop11;
			}
			
		}
		_loop11:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::primaryExpr() {
	expr x;
	
	try {      // for error handling
		x=atomExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::atomExpr() {
	expr x;
	
	try {      // for error handling
		x=postfixExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::postfixExpr() {
	expr x;
	
	try {      // for error handling
		x=atoms();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::atoms() {
	expr x;
	
	
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case NUMBER:
		case INT:
		{
			x=final_values();
			break;
		}
		case ID:
		{
			x=star_or_symbol();
			break;
		}
		case OR:
		{
			match(OR);
			x=term();
			x = abs(x);
			match(OR);
			break;
		}
		case LPAREN:
		{
			match(LPAREN);
			x=term();
			match(RPAREN);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::final_values() {
	expr x;
	
	try {      // for error handling
		x=atom_number();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

expr  parser_sym_arrow::star_or_symbol() {
	expr x;
	
	symbol sym;
	expr y;
	std::vector<expr> args;
	
	
	try {      // for error handling
		sym=scoped_symbol();
		x = sym;
		{
		switch ( LA(1)) {
		case LBRACK:
		{
			match(LBRACK);
			{
			switch ( LA(1)) {
			case PLUS:
			case MINUS:
			case OR:
			case LPAREN:
			case ID:
			case NUMBER:
			case INT:
			{
				y=term();
				args.push_back(y);
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == COMMA)) {
						match(COMMA);
						y=term();
						args.push_back(y);
					}
					else {
						goto _loop25;
					}
					
				}
				_loop25:;
				} // ( ... )*
				break;
			}
			case RBRACK:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(RBRACK);
			x = make_function(sym, args);
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case PLUS:
		case MINUS:
		case MULT:
		case DIV:
		case POWER:
		case OR:
		case RPAREN:
		case COMMA:
		case RBRACK:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

symbol  parser_sym_arrow::scoped_symbol() {
	symbol x;
	
	try {      // for error handling
		x=atom_symbol();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
	}
	return x;
}

symbol  parser_sym_arrow::atom_symbol() {
	symbol x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		i2 = LT(1);
		match(ID);
		x = symbol(i2->getText());
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_4);
	}
	return x;
}

expr  parser_sym_arrow::atom_number() {
	expr x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	expr tmp;
	int ival;
	
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			i = LT(1);
			match(NUMBER);
			x = get_number(i->getText());
			break;
		}
		case INT:
		{
			ival=atom_int();
			x = expr(ival);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

int  parser_sym_arrow::atom_int() {
	int x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	x = 0;
	
	
	try {      // for error handling
		i = LT(1);
		match(INT);
		x = get_int(i->getText());
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		recover(ex,_tokenSet_3);
	}
	return x;
}

void parser_sym_arrow::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* parser_sym_arrow::tokenNames[] = {
	"<0>",
	"end of file",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\'+\'",
	"\'-\'",
	"\'*\'",
	"\'/\'",
	"\'^\'",
	"|",
	"\'(\'",
	"\')\'",
	"an identifier",
	"\'[\'",
	"\',\'",
	"\']\'",
	"number",
	"integer",
	"\'{\'",
	"\'}\'",
	"\';\'",
	"ESC",
	"white space",
	"new line",
	"not new line",
	"not new line",
	"comment",
	"SL_COMMENT",
	"SL_DCOMMENT",
	"ML_COMMENT",
	"string",
	"string",
	"continuation",
	"digit",
	"exponent",
	"complex",
	"double dot",
	"dot",
	0
};

const unsigned long parser_sym_arrow::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_1_data_[] = { 51714UL, 0UL, 0UL, 0UL };
// EOF OR RPAREN COMMA RBRACK 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_2_data_[] = { 51762UL, 0UL, 0UL, 0UL };
// EOF PLUS MINUS OR RPAREN COMMA RBRACK 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_3_data_[] = { 52210UL, 0UL, 0UL, 0UL };
// EOF PLUS MINUS MULT DIV POWER OR RPAREN COMMA RBRACK 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_4_data_[] = { 60402UL, 0UL, 0UL, 0UL };
// EOF PLUS MINUS MULT DIV POWER OR RPAREN LBRACK COMMA RBRACK 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_4(_tokenSet_4_data_,4);


