/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "sym_arrow/functions/contexts.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/nodes/symbol.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

#include "antlr/antlr.h"
#include "grammar/output/lexer_sym_arrow.hpp"
#include "grammar/output/parser_sym_arrow.hpp"

namespace sym_arrow
{

class parser
{
    private:
        using filter_type   = antlr::TokenStreamHiddenTokenFilter;

    private:
        region::region      m_reg;
        std::istringstream* m_is;
        lexer_sym_arrow*    m_lexer;
        filter_type*        m_filter;
        parser_sym_arrow*   m_parser;

    public:
        parser(const std::string& v);
        ~parser();

    private:
        parser(const parser&) = delete;
        parser& operator=(const parser&) = delete;

    public:
        expr    make_expr();
        index   make_index();
        symbol  make_symbol();

        void    make_def();

    private:
        template<class T>
        static T    make(const std::function<T ()>& f);
};

parser::parser(const std::string& v)
    : m_is(nullptr), m_lexer(nullptr), m_filter(nullptr), m_parser(nullptr)
{    
    m_is        = new std::istringstream(v);
    m_lexer     = new lexer_sym_arrow(&m_reg, *m_is);
    m_filter    = new antlr::TokenStreamHiddenTokenFilter(&m_reg, *m_lexer);

    m_filter->hide(lexer_sym_arrow::WHITESPACE);
    m_filter->hide(lexer_sym_arrow::NEWLINE);
    m_filter->hide(lexer_sym_arrow::COMMENT);
    m_filter->hide(lexer_sym_arrow::SL_DCOMMENT);
    m_filter->hide(lexer_sym_arrow::SL_COMMENT);
    m_filter->hide(lexer_sym_arrow::ML_COMMENT);
    m_filter->hide(lexer_sym_arrow::NOT_NEWLINE);    
    m_filter->hide(lexer_sym_arrow::CONTINUATION);    

    m_parser = new parser_sym_arrow(&m_reg, *m_filter);

    m_parser->init(&std::cout);
}

parser::~parser()
{
    delete m_parser;
    delete m_filter;
    delete m_lexer;
    delete m_is;
};

template<class T>
static T parser::make(const std::function<T ()>& f)
{
    try
    {
        T ex = f();
        return ex;
    }
    catch (antlr::CharStreamIOException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
    catch (antlr::RecognitionException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
    catch (antlr::TokenStreamIOException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
    catch (antlr::TokenStreamRetryException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        throw std::runtime_error(e.getMessage());
    }
}

expr parser::make_expr()
{
    parser_sym_arrow* p = m_parser;

    return make<expr>([p, this](){return p->stringUnit();});
};

index parser::make_index()
{
    parser_sym_arrow* p = m_parser;

    return make<index>([p, this](){return p->index_def();});
}

symbol parser::make_symbol()
{
    parser_sym_arrow* p = m_parser;

    return make<symbol>([p, this](){return p->symbol_def();});
}

void parser::make_def()
{
    parser_sym_arrow* p = m_parser;

    make<int>([p, this](){p->any_def(); return 0;});
};

expr sym_arrow::parse(const std::string& v)
{
    if (v.size() == 0)
        return ast::scalar_rep::make_zero();

    parser p(v);

    expr ex = p.make_expr();
    return ex;
};

void sym_arrow::parse_def(const std::string& v)
{
    if (v.size() == 0)
        return;

    parser p(v);

    p.make_def();
};

index sym_arrow::parse_index(const std::string& v)
{
    parser p(v);

    index i = p.make_index();
    return i;
}

symbol sym_arrow::parse_sym(const std::string& v)
{
    parser p(v);

    symbol s = p.make_symbol();
    return s;
}

};