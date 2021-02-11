/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/nodes/add_expr.h"
#include "sym_arrow/nodes/mult_expr.h"
#include "sym_arrow/nodes/function_expr.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/exception.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/functions/expr_functions.h"

//#include <vld.h>

namespace sym_arrow
{

expr::expr()
{};

expr::expr(const expr& other)
    : m_ptr(other.m_ptr)
{};

expr::expr(expr&& other)
    : m_ptr(std::move(other.m_ptr))
{};

expr::expr(const ptr_type& p)  
    : m_ptr(p)
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        if (check_expression(*this) == false)
        {
            disp_nocannonize(std::cout, *this, true);
            assertion(check_expression(*this) == true,"invalid expression");
        };
    #endif
};

expr::expr(ptr_type&& p)
    : m_ptr(std::move(p))
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        if (check_expression(*this) == false)
        {
            disp_nocannonize(std::cout, *this, true);
            assertion(check_expression(*this) == true,"invalid expression");
        };
    #endif
};

expr::expr(const ast::expr_base* p)
    : m_ptr(p,sym_dag::copy_t())
{
    #ifdef SYM_ARROW_DEBUG_EXPR
        if (check_expression(*this) == false)
        {
            disp_nocannonize(std::cout, *this, true);
            assertion(check_expression(*this) == true,"invalid expression");
        };
    #endif
};

expr::expr(const scalar& val)                 
    : m_ptr(val.get_ptr())
{};

expr::expr(const value& p)
    : m_ptr(scalar(p).get_ptr())
{};

expr::expr(double v)
    : m_ptr(scalar(value::make_value(v)).get_ptr())
{};

expr::expr(const symbol& val)
    :expr(val.get_ptr())
{};

expr::expr(const add_expr& val)
    :expr(val.get_expr())
{};

expr::expr(const mult_expr& val)
    :expr(val.get_expr())
{};

expr::expr(const function_expr& val)
    :expr(val.get_expr())
{};

expr::~expr()
{};

expr& expr::operator=(const expr& other)
{
    m_ptr       = other.m_ptr;
    return *this;
};

expr& expr::operator=(expr&& other)
{
    m_ptr       = std::move(other.m_ptr);
    return *this;
};

bool expr::is_cannonized() const
{
    return ast::cannonize().is_cannonized(*this);
};

void expr::cannonize(bool do_cse) const
{
    if (ast::cannonize().is_cannonized(*this) == true)
        return;

    const_cast<expr&>(*this) = ast::cannonize().make(*this, do_cse);
};

bool operator==(const expr& v1, const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() == v2.get_ptr();
};

bool operator!=(const expr& v1,const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() != v2.get_ptr();
};

bool operator>(const expr& v1,const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() > v2.get_ptr();
};

bool operator<(const expr& v1,const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() < v2.get_ptr();
};

bool operator<=(const expr& v1,const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() < v2.get_ptr();
};

bool operator>=(const expr& v1,const expr& v2)
{
    v1.cannonize(do_cse_default);
    v2.cannonize(do_cse_default);

    return v1.get_ptr() > v2.get_ptr();
};

expr sym_arrow::function(const symbol& sym)
{
    return function(sym, nullptr, 0);
};

expr sym_arrow::function(const symbol& sym, const expr& arg1)
{
    expr args[] = {arg1};
    return function(sym, args, 1);
};

expr sym_arrow::function(const symbol& sym, const expr& arg1, const expr& arg2)
{
    expr args[] = {arg1, arg2};
    return function(sym, args, 2);
};

expr sym_arrow::function(const symbol& sym, const std::vector<expr>& arg)
{
    return function(sym, arg.data(), arg.size());
};

expr sym_arrow::function(const symbol& sym, const expr* arg, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        arg[i].cannonize(do_cse_default);

    using info              = ast::function_rep_info;

    info f_info             = info(sym.get_ptr().get(), n, arg);
    ast::expr_ptr ep        = ast::function_rep::make(f_info);

    return expr(ep);
};

};
