/* 
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

expr sym_arrow::parse(const std::string& v)
{
    if (v.size() == 0)
        return ast::scalar_rep::make_zero();

    region::region reg;

    std::istringstream is(v);

    lexer_sym_arrow lexer(&reg,is);

    antlr::TokenStreamHiddenTokenFilter filter(&reg,lexer);

    filter.hide(lexer_sym_arrow::WHITESPACE);
    filter.hide(lexer_sym_arrow::NEWLINE);
    filter.hide(lexer_sym_arrow::COMMENT);
    filter.hide(lexer_sym_arrow::SL_DCOMMENT);
    filter.hide(lexer_sym_arrow::SL_COMMENT);
    filter.hide(lexer_sym_arrow::ML_COMMENT);
    filter.hide(lexer_sym_arrow::NOT_NEWLINE);    
    filter.hide(lexer_sym_arrow::CONTINUATION);    

    parser_sym_arrow parser(&reg,filter);

    parser.init(&std::cout);

    try
    {
        expr ex = parser.stringUnit();
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
};

};