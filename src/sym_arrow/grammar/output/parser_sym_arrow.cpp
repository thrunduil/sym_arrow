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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::term() {
	expr x;
	
	try {      // for error handling
		x=addExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	return x;
}

void parser_sym_arrow::any_def() {
	
	try {      // for error handling
		switch ( LA(1)) {
		case SET:
		{
			set_def();
			break;
		}
		case TYPE:
		{
			type_def();
			break;
		}
		case SYM:
		{
			sym_def();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

void parser_sym_arrow::set_def() {
	
	identifier sym;
	set ex;
	
	
	try {      // for error handling
		match(SET);
		sym=sym_name();
		ex=set_initializer();
		if ( inputState->guessing==0 ) {
			def_set(sym, ex);
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

void parser_sym_arrow::type_def() {
	
	identifier sym;
	
	
	try {      // for error handling
		match(TYPE);
		sym=sym_name();
		if ( inputState->guessing==0 ) {
			def_type(sym);
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

void parser_sym_arrow::sym_def() {
	
	identifier sym;
	std::vector<identifier> args;
	identifier t;
	
	
	try {      // for error handling
		match(SYM);
		sym=sym_name();
		{
		switch ( LA(1)) {
		case LANGLE:
		{
			index_postfix_def(args);
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case COLON:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		switch ( LA(1)) {
		case COLON:
		{
			t=type_postfix();
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
			def_sym(sym, args, t);
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
}

identifier  parser_sym_arrow::sym_name() {
	identifier x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		i2 = LT(1);
		match(ID);
		if ( inputState->guessing==0 ) {
			x = identifier(i2->getText());
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	return x;
}

set  parser_sym_arrow::set_initializer() {
	set x;
	
	try {      // for error handling
		match(DOTEQ);
		x=set_literal();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
	return x;
}

void parser_sym_arrow::index_postfix_def(
	std::vector<identifier>& args
) {
	
	identifier y;
	
	
	try {      // for error handling
		match(LANGLE);
		{
		switch ( LA(1)) {
		case ID:
		{
			y=sym_name();
			if ( inputState->guessing==0 ) {
				args.push_back(y);
			}
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					y=sym_name();
					if ( inputState->guessing==0 ) {
						args.push_back(y);
					}
				}
				else {
					goto _loop49;
				}
				
			}
			_loop49:;
			} // ( ... )*
			break;
		}
		case RANGLE:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RANGLE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_3);
		} else {
			throw;
		}
	}
}

identifier  parser_sym_arrow::type_postfix() {
	identifier y;
	
	try {      // for error handling
		match(COLON);
		y=sym_name();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
	return y;
}

set  parser_sym_arrow::set_literal() {
	set x;
	
	identifier y;
	std::vector<identifier> args;
	
	
	try {      // for error handling
		match(LCURL);
		{
		switch ( LA(1)) {
		case ID:
		{
			y=sym_name();
			if ( inputState->guessing==0 ) {
				args.push_back(y);
			}
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					y=sym_name();
					if ( inputState->guessing==0 ) {
						args.push_back(y);
					}
				}
				else {
					goto _loop55;
				}
				
			}
			_loop55:;
			} // ( ... )*
			break;
		}
		case RCURL:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RCURL);
		if ( inputState->guessing==0 ) {
			x = make_set(args);
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
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
				if ( inputState->guessing==0 ) {
					x = std::move(x) + std::move(tmp);
				}
				break;
			}
			case MINUS:
			{
				match(MINUS);
				tmp=multExpr();
				if ( inputState->guessing==0 ) {
					x = std::move(x) - std::move(tmp);
				}
				break;
			}
			default:
			{
				goto _loop12;
			}
			}
		}
		_loop12:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
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
				if ( inputState->guessing==0 ) {
					x = std::move(x) * std::move(tmp);
				}
				break;
			}
			case DIV:
			{
				match(DIV);
				tmp=sgnAtom();
				if ( inputState->guessing==0 ) {
					x = std::move(x) / std::move(tmp);
				}
				break;
			}
			default:
			{
				goto _loop15;
			}
			}
		}
		_loop15:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_4);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
				x = - std::move(x);
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
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
				if ( inputState->guessing==0 ) {
					x = make_power(std::move(x),std::move(tmp));
				}
			}
			else {
				goto _loop18;
			}
			
		}
		_loop18:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::primaryExpr() {
	expr x;
	
	try {      // for error handling
		x=atomExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::atomExpr() {
	expr x;
	
	try {      // for error handling
		x=postfixExpr();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::postfixExpr() {
	expr x;
	
	try {      // for error handling
		x=atoms();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
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
			x=symbol_postfix();
			break;
		}
		case OR:
		{
			match(OR);
			x=term();
			if ( inputState->guessing==0 ) {
				x = abs(x);
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::final_values() {
	expr x;
	
	try {      // for error handling
		x=atom_number();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

expr  parser_sym_arrow::symbol_postfix() {
	expr x;
	
	identifier sym;    
	std::vector<expr> args;
	
	
	try {      // for error handling
		sym=sym_name();
		{
		switch ( LA(1)) {
		case LBRACK:
		{
			function_postfix(args);
			if ( inputState->guessing==0 ) {
				x = make_function(sym, args);
			}
			break;
		}
		case LANGLE:
		{
			index_postfix(args);
			if ( inputState->guessing==0 ) {
				x = make_indexed(sym, args);
			}
			break;
		}
		case COLON:
		{
			x=indexer_postfix(sym);
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
		case RANGLE:
		{
			if ( inputState->guessing==0 ) {
				x = make_symbol(sym);
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

void parser_sym_arrow::function_postfix(
	std::vector<expr>& args
) {
	
	expr y;
	
	
	try {      // for error handling
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
			if ( inputState->guessing==0 ) {
				args.push_back(y);
			}
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					y=term();
					if ( inputState->guessing==0 ) {
						args.push_back(y);
					}
				}
				else {
					goto _loop41;
				}
				
			}
			_loop41:;
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

void parser_sym_arrow::index_postfix(
	std::vector<expr>& args
) {
	
	expr y;
	
	
	try {      // for error handling
		match(LANGLE);
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
			if ( inputState->guessing==0 ) {
				args.push_back(y);
			}
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == COMMA)) {
					match(COMMA);
					y=term();
					if ( inputState->guessing==0 ) {
						args.push_back(y);
					}
				}
				else {
					goto _loop45;
				}
				
			}
			_loop45:;
			} // ( ... )*
			break;
		}
		case RANGLE:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RANGLE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
}

index  parser_sym_arrow::indexer_postfix(
	const identifier& sym
) {
	index x;
	
	identifier y;
	
	
	try {      // for error handling
		match(COLON);
		y=sym_name();
		if ( inputState->guessing==0 ) {
			x = make_indexer(sym, y);
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	return x;
}

index  parser_sym_arrow::index_def() {
	index x;
	
	identifier sym;    
	std::vector<expr> args;
	
	
	try {      // for error handling
		sym=sym_name();
		x=indexer_postfix(sym);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
	}
	return x;
}

symbol  parser_sym_arrow::symbol_def() {
	symbol x;
	
	identifier sym;    
	std::vector<expr> args;
	
	
	try {      // for error handling
		sym=sym_name();
		{
		switch ( LA(1)) {
		case LANGLE:
		{
			index_postfix(args);
			if ( inputState->guessing==0 ) {
				x = make_indexed(sym, args);
			}
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		{
			if ( inputState->guessing==0 ) {
				x = make_symbol(sym);
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_0);
		} else {
			throw;
		}
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
			if ( inputState->guessing==0 ) {
				x = get_number(i->getText());
			}
			break;
		}
		case INT:
		{
			ival=atom_int();
			if ( inputState->guessing==0 ) {
				x = expr(ival);
			}
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
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
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
		if ( inputState->guessing==0 ) {
			x = get_int(i->getText());
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
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
	"\"set\"",
	"\"type\"",
	"\"sym\"",
	":=",
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
	"\'<\'",
	"\':\'",
	"\',\'",
	"\']\'",
	"\'>\'",
	"\'{\'",
	"\'}\'",
	"number",
	"integer",
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
const unsigned long parser_sym_arrow::_tokenSet_1_data_[] = { 7380994UL, 0UL, 0UL, 0UL };
// EOF OR RPAREN COMMA RBRACK RANGLE 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_2_data_[] = { 25083778UL, 0UL, 0UL, 0UL };
// EOF DOTEQ PLUS MINUS MULT DIV POWER OR RPAREN LBRACK LANGLE COLON COMMA 
// RBRACK RANGLE RCURL 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_3_data_[] = { 524290UL, 0UL, 0UL, 0UL };
// EOF COLON 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_4_data_[] = { 7381762UL, 0UL, 0UL, 0UL };
// EOF PLUS MINUS OR RPAREN COMMA RBRACK RANGLE 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long parser_sym_arrow::_tokenSet_5_data_[] = { 7388930UL, 0UL, 0UL, 0UL };
// EOF PLUS MINUS MULT DIV POWER OR RPAREN COMMA RBRACK RANGLE 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_sym_arrow::_tokenSet_5(_tokenSet_5_data_,4);


