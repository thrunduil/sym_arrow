header 
{
    //parser.hpp includes
    #include "grammar/parser_include.h"
}

options 
{
    language        = "Cpp";
    genHashLines    = false; 
}
{
    //parser.cpp includes
}

//------------------------------------------------------------------------------
//                  PARSER
//------------------------------------------------------------------------------
class           parser_sym_arrow extends Parser;
options     
{       
    buildAST        = false;  
    exportVocab     = sym_arrow_vocabulary;
    k                 = 1;
}                               

{    
    //parser members
    #include "grammar/parser_members.h"
}

//------------------------------------------------------------------------------
//                  START
//------------------------------------------------------------------------------
stringUnit returns[expr x]
{}
    :    x = term        
;

//------------------------------------------------------------------------------
//                  DEFINITIONS
//------------------------------------------------------------------------------
any_def
{}
    :   set_def
    |   type_def
    |   sym_def
    |   fun_def
;

set_def 
{
    identifier sym;
    set ex;
}
    :   SET
        sym   = sym_name
        ex    = set_initializer                 { def_set(sym, ex); }
;

type_def 
{
    identifier sym;
}
    :   TYPE
        sym   = sym_name                        { def_type(sym); }
;

sym_def 
{
    identifier sym;

    std::vector<identifier> args;
    type t;
}
    :   SYM
        sym   = sym_name
        symbol_postfix_def[args, t] { def_sym(sym, args, t); }
;

fun_def 
{
    identifier      sym;
    formal_arg      arg;
    type            t;
    std::vector<formal_arg> args;
}
    :   FUN
        sym   = sym_name

        LBRACK
        (
            arg = function_arg          { args.push_back(arg); }
            (
                COMMA
                arg = function_arg      { args.push_back(arg); }
            )*
        )?
        RBRACK

        type_postfix[t]                 { def_fun(sym, args, t); }
;

function_arg returns[formal_arg x]
{
    identifier      id;
    type            t;
}
    :   id = sym_name
        type_postfix[t]                 { x = formal_arg(id, t);}
;

set_initializer returns[set x]
{}
    : DOTEQ
      x = set_literal
;

//------------------------------------------------------------------------------
//                  EXPRESSIONS
//------------------------------------------------------------------------------
term returns[expr x]
{}
    :    x = addExpr
;

addExpr returns[expr x]
{
    expr tmp;
}
    :   x = multExpr
        ( 
            PLUS tmp = multExpr                 { x = std::move(x) + std::move(tmp); }
            |MINUS tmp = multExpr               { x = std::move(x) - std::move(tmp); }
        )*
;
multExpr returns[expr x]
{
    expr tmp;
}
    :   x = sgnAtom
        (
            MULT tmp = sgnAtom                  { x = std::move(x) * std::move(tmp); }
            |DIV tmp = sgnAtom                  { x = std::move(x) / std::move(tmp); }
        )*
;
powExpr returns[expr x]
{
    expr tmp;
}     
    :   x = primaryExpr
        (
            options {greedy = true;}:
            POWER  
            tmp = sgnAtom                       { x = make_power(std::move(x),std::move(tmp)); }
        )*
;
sgnAtom returns[expr x]
{
    expr tmp;
}     
    :   (
                MINUS   x = powExpr             { x = - std::move(x); }
            |   PLUS    x = powExpr
            |   x = powExpr
        )
;

primaryExpr returns[expr x]
{}
    :    x = atomExpr    
;

//------------------------------------------------------------------------------
//                  ATOMS
//------------------------------------------------------------------------------
atomExpr returns[expr x]
{}
    :   x = postfixExpr
;

postfixExpr returns[expr x]
{}
    :    x = atoms
;

atoms returns[expr x]
{    
}
    :   (
            x = final_values
        |   x = symbol_postfix
        |   OR
                x = term                    { x = abs(x); }
            OR
        |   LPAREN  
                x = term
            RPAREN
        )
;


sym_name returns[identifier x]
{}
    :   i2:ID                               { x = identifier(i2->getText()); }
;

symbol_postfix returns[expr x]
{
    identifier sym;  
    type t;
    std::vector<expr> args;
}
    :   sym = sym_name
        (
            (LBRACK) 
                => function_postfix[args]   { x = make_function(sym, args); }
        |   (LANGLE | COLON) 
                => index_postfix[args, t]   { x = make_symbol(sym, args, t); }
        |                                   { x = make_symbol(sym, args, t); }
        )
;

symbol_def returns[symbol x]
{
    identifier sym;    
    std::vector<expr> args;
    type t;    
}
    :   sym = sym_name
        index_postfix[args, t]          { x = make_symbol(sym, args, t); }
;

function_postfix[std::vector<expr>& args]
{
    expr y;
}
    :   LBRACK
        (
            y = term                    { args.push_back(y); }
            (
                COMMA
                y = term                { args.push_back(y); }
            )*
        )?
        RBRACK                         
;

index_postfix[std::vector<expr>& args, type& t]
{
    expr y;
    bool is_const = false;
}
    :   (
            LANGLE
            (
                y = term                { args.push_back(y); }
                (
                    COMMA
                    y = term            { args.push_back(y); }
                )*
            )?
            RANGLE
        )?

        (
            type_postfix[t]
        )?
;

symbol_postfix_def[std::vector<identifier>& args, type& t]
{
    identifier y;
    bool is_const = false;
}
    :   (
            LANGLE
            (
                y = sym_name            { args.push_back(y); }
                (
                    COMMA
                    y = sym_name        { args.push_back(y); }
                )*
            )?
            RANGLE                         
        )?

        type_postfix[t]
;

type_postfix[type& t]
{
    identifier y;
    bool is_const = false;
}
    :   COLON
        (
            CONST                       { is_const = true; }
        )?
        y = sym_name                    { t = type(y, is_const); }
;

set_literal returns[set x]
{
    identifier y;
    std::vector<identifier> args;
}
    :   LCURL
        (
            y = sym_name                { args.push_back(y); }
            (
                COMMA
                y = sym_name            { args.push_back(y); }
            )*
        )?
        RCURL                           { x = make_set(args); }
;

final_values returns[expr x]
{}
    :   x = atom_number        
;

atom_number returns[expr x]
{
    expr tmp;
    int ival;
}
    :   (
            i:NUMBER                        { x = get_number(i->getText()); }
        |   ival = atom_int                 { x = expr(ival);}
        )
;
atom_int returns[int x]
{
    x = 0;
}
    :    i:INT                              { x = get_int(i->getText()); }
;


//------------------------------------------------------------------------------
//                  LEXER
//------------------------------------------------------------------------------
{
    #include "grammar/lexer_include.h"
}

class  lexer_sym_arrow extends Lexer;

options
{
    exportVocab                         = sym_arrow_vocabulary;
    charVocabulary                      = '\3'..'\377';
    k                                   = 3;
    defaultErrorHandler                 = true;
    genHashLines                        = false;  
    
    codeGenMakeSwitchThreshold          = 5; 
    codeGenBitsetTestThreshold          = 5;    
}

tokens 
{
    //keywords

    SET                     = "set";
    SYM                     = "sym";
    TYPE                    = "type";
    FUN                     = "fun";
    CONST                   = "const";
}

LPAREN  options {paraphrase = "'('";}
            :       '('    ;
RPAREN  options {paraphrase = "')'";}
            :       ')'    ;
LBRACK  options {paraphrase = "'['";}
            :       '[' ;
RBRACK  options {paraphrase = "']'";}
            :       ']' ;
LCURL   options {paraphrase = "'{'";}
            :       '{' ;
RCURL   options {paraphrase = "'}'";}
            :       '}' ;
LANGLE   options {paraphrase = "'<'";}
            :       '<' ;
RANGLE   options {paraphrase = "'>'";}
            :       '>' ;
MULT    options {paraphrase = "'*'";}
            :       '*'    ;
DIV     options {paraphrase = "'/'";}
            :       '/' | '\\';
PLUS    options {paraphrase = "'+'";}
            :       '+'    ;
MINUS   options {paraphrase = "'-'";}
            :       '-'    ;
POWER   options {paraphrase = "'^'";}
            :       '^'    ;
SEMI    options {paraphrase = "';'";}
            :       ';'    ;
COMMA   options {paraphrase = "','";}
            :       ','    ;

COLON   options {paraphrase = "':'";}
            :       ':'    ;
                        
OR      options {paraphrase = "|";}
            :       "|";

DOTEQ   options {paraphrase = ":=";}
            :       ":=";

            
protected
ESC    :    '\\'
        (    'n'  
        |    'r'  
        |    't'  
        |    'b'  
        |    'f'  
        |    '"'  
        |    '\'' 
        |   '/'  
        |    '\\' 
        )
;

ID options 
{
testLiterals=true;
paraphrase = "an identifier";
}
    :       ('a'..'z'|'A'..'Z') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*;
            
WHITESPACE    options{ paraphrase = "white space"; } 
    :    NON_NEWLINE_WHITESPACE (NON_NEWLINE_WHITESPACE)*
;
NEWLINE    options{ paraphrase = "new line"; } 
    :    ( 
              '\r'                                                  // MacOS-style newline
              ( options { generateAmbigWarnings=false; } 
                : '\n'                                              // DOS/Windows style newline
              )?
            | '\n'                                                  // UNIX-style newline
        )                    
                                                                    { newline(); }
;
protected NOT_NEWLINE options{ paraphrase = "not new line"; } 
    :     ~( '\r' | '\n' )
;

protected NON_NEWLINE_WHITESPACE options{ paraphrase = "not new line"; } 
    :    '\t'                                                       // horiz_tab
    |    ' '                                                        // space 
    |     '\f'                                                      // form_feed
;

COMMENT options{ paraphrase = "comment"; } 
    :    (                    
            ("%{") => ML_COMMENT                                    { _ttype = ML_COMMENT; }
            | 
                "%"                                                 { _ttype = SL_COMMENT; }
                (
                    options {greedy = true;}:                       { _ttype = SL_DCOMMENT; }
                    "%"
                )?
                ( NOT_NEWLINE )* (NEWLINE)?
        )
;

protected SL_COMMENT : ;
protected SL_DCOMMENT : ;
protected ML_COMMENT  
{
    int ML_depth = 0;
}           
    :     "%{"                                                      {ML_depth=1;}
        ( 
            options { generateAmbigWarnings=false; }:
              ('"' | '`') => STRING_LITERAL
            | { LA(2)!='}' && LA(2)!='{' }? '%'
            | { LA(2)=='}' && ML_depth!=1 }? '%'                    {ML_depth--;} //remove one nesting  level
            | "%{"                                                  {ML_depth++;}
            | NEWLINE
            | ~('%' | '\r' | '\n')
        )*
        "%}"                                                            
;

STRING_LITERAL options{ paraphrase = "string"; }  
    :      (
                ('"' '"' '"') => MULTILINE_STRING                   { _ttype = MULTILINE_STRING; }
            |     '`' (  ~( '`' | '\r' | '\n' ) )* '`'
        )
;
protected MULTILINE_STRING options{ paraphrase = "string"; }
    :    (
            '"' '"' '"'
            (     
                options {greedy = true;}:
                {LA(2)!='"' && LA(3)!='"'}? '"'
                |~('"'| '\r' | '\n')
                | NEWLINE
            )*
            '"' '"' '"'
        )
;
protected CONTINUATION options{ paraphrase = "continuation"; }  
    :    "..." ( NOT_NEWLINE )* NEWLINE                                    
;

NUMBER options{ paraphrase = "number"; }     
    :      ( (DIGIT)+ ('.' DIGIT | 'e' | 'E') )                              
            => (DIGIT)+ ('.' (DIGIT)+ (EXPONENT)?| EXPONENT)
        |     ("...") => CONTINUATION                               { _ttype = CONTINUATION; }                
        |     ("..") => ".."                                        { _ttype = DDOT; }
        |     '.'                                                   { _ttype = DOT; }
            (
                (DIGIT)+ (EXPONENT)?                                { _ttype = NUMBER; }              
            )?
        |   '0'..'9' (DIGIT)*                                       { _ttype = INT;}
;
protected DIGIT options{ paraphrase = "digit"; } 
    :     '0'..'9';
    
protected EXPONENT  options { paraphrase = "exponent"; }  
    :    ('e' | 'E') ('+' | '-')? (DIGIT)+;
    
protected COMPLEX options{ paraphrase = "complex"; }: ;
protected INT options{ paraphrase = "integer"; }: ;
protected DDOT options{ paraphrase = "double dot"; }: ;
protected DOT options{ paraphrase = "dot"; }: ;
protected EOF options{ paraphrase = "end of file"; }: ;