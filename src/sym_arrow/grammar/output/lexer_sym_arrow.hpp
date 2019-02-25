#ifndef INC_lexer_sym_arrow_hpp_
#define INC_lexer_sym_arrow_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "sym_arrow.g" -> "lexer_sym_arrow.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "sym_arrow_vocabularyTokenTypes.hpp"
#include <antlr/CharScanner.hpp>

    //parser.hpp includes
    #include "grammar/parser_include.h"

class CUSTOM_API lexer_sym_arrow : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public sym_arrow_vocabularyTokenTypes
{
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	lexer_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(std)istream& in);
	lexer_sym_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	lexer_sym_arrow(antlr::pool_parser pool, const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mLPAREN(bool _createToken);
	public: void mRPAREN(bool _createToken);
	public: void mLBRACK(bool _createToken);
	public: void mRBRACK(bool _createToken);
	public: void mLCURL(bool _createToken);
	public: void mRCURL(bool _createToken);
	public: void mLANGLE(bool _createToken);
	public: void mRANGLE(bool _createToken);
	public: void mMULT(bool _createToken);
	public: void mDIV(bool _createToken);
	public: void mPLUS(bool _createToken);
	public: void mMINUS(bool _createToken);
	public: void mPOWER(bool _createToken);
	public: void mSEMI(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mOR(bool _createToken);
	protected: void mESC(bool _createToken);
	public: void mID(bool _createToken);
	public: void mWHITESPACE(bool _createToken);
	protected: void mNON_NEWLINE_WHITESPACE(bool _createToken);
	public: void mNEWLINE(bool _createToken);
	protected: void mNOT_NEWLINE(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	protected: void mML_COMMENT(bool _createToken);
	protected: void mSL_COMMENT(bool _createToken);
	protected: void mSL_DCOMMENT(bool _createToken);
	public: void mSTRING_LITERAL(bool _createToken);
	protected: void mMULTILINE_STRING(bool _createToken);
	protected: void mCONTINUATION(bool _createToken);
	public: void mNUMBER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mEXPONENT(bool _createToken);
	protected: void mCOMPLEX(bool _createToken);
	protected: void mINT(bool _createToken);
	protected: void mDDOT(bool _createToken);
	protected: void mDOT(bool _createToken);
	protected: void mEOF(bool _createToken);
private:
	
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
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
};

#endif /*INC_lexer_sym_arrow_hpp_*/
