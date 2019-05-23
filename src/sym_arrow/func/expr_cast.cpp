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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/add_expr.h"
#include "sym_arrow/nodes/mult_expr.h"
#include "sym_arrow/nodes/function_expr.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

sym_arrow::ast::term_types sym_arrow::get_expression_type(const expr& ex)
{
    return static_cast<sym_arrow::ast::term_types>(ex.get_ptr()->get_code());
};

const sym_arrow::scalar& sym_arrow::cast_scalar(const expr& ex)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(ex.get_ptr()->isa<ast::scalar_rep>(), "invalid cast");
    #endif
    
    return reinterpret_cast<const sym_arrow::scalar&>(ex);
};

const sym_arrow::symbol& sym_arrow::cast_symbol(const expr& ex)
{    
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(ex.get_ptr()->isa<ast::symbol_rep>(), "invalid cast");
    #endif
    
    return reinterpret_cast<const sym_arrow::symbol&>(ex);
};

const sym_arrow::add_expr& sym_arrow::cast_add(const expr& ex)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(ex.get_ptr()->isa<ast::add_rep>(), "invalid cast");
    #endif
    
    return reinterpret_cast<const sym_arrow::add_expr&>(ex);
};

const sym_arrow::mult_expr& sym_arrow::cast_mult(const expr& ex)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(ex.get_ptr()->isa<ast::mult_rep>(), "invalid cast");
    #endif
    
    return reinterpret_cast<const sym_arrow::mult_expr&>(ex);
};

const sym_arrow::function_expr& sym_arrow::cast_function(const expr& ex)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        assertion(ex.get_ptr()->isa<ast::function_rep>(), "invalid cast");
    #endif
    
    return reinterpret_cast<const sym_arrow::function_expr&>(ex);
};

