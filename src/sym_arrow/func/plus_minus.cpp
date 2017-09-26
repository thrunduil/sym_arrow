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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "process_scalar.h"
#include "compound.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace details
{

struct do_plus : public process_scalar2<do_plus>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, const value& b)
    {
        ret = expr(a + b);
    }

    static void eval(expr& ret, const value& a, ast::expr_handle b)
    {
        return compound::plus_build(ret, a, value::make_one(), b);
    };

    static void eval(expr& ret, ast::expr_handle a, const value& b)
    {
        return compound::plus_build(ret, b, value::make_one(), a);
    };

    static void eval(expr& ret, ast::expr_handle a, ast::expr_handle b)
    {
        return compound::plus_build(ret, value::make_zero(), value::make_one(), 
                                         a, value::make_one(), b);
    }
};

struct do_minus : public process_scalar2<do_minus>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, const value& b)
    {
        ret = expr(a - b);
    }

    static void eval(expr& ret, const value& a, ast::expr_handle b)
    {
        return compound::plus_build(ret, a, value::make_minus_one(), b);
    }

    static void eval(expr& ret, ast::expr_handle a, const value& b)
    {
        return compound::plus_build(ret, -b, value::make_one(), a);
    }

    static void eval(expr& ret, ast::expr_handle a, ast::expr_handle b)
    {
        return compound::plus_build(ret, value::make_zero(), value::make_one(), a, 
                                    value::make_minus_one(), b);
    }
};

struct do_uminus : public process_scalar1<do_uminus>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a)
    {
        ret = expr(-a);
    }

    static void eval(expr& ret, ast::expr_handle a)
    {
        return compound::plus_build(ret, value::make_zero(), value::make_minus_one(), a);
    };
};

}};

sym_arrow::expr sym_arrow::operator+(const expr& a, const expr& b)
{    
    expr ret;
    details::do_plus().make(ret, a,b);
    return ret;
}

sym_arrow::expr sym_arrow::operator+(const expr& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);
    
    expr ret = operator+(a,b);
    
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator+(expr&& a0, const expr& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    
    expr ret = operator+(a,b);
    
    a.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator+(expr&& a0, expr&& b0)
{
    expr a(std::move(a0));
    expr b(std::move(b0));

    a.get_ptr()->make_temporary(true);
    b.get_ptr()->make_temporary(true);

    expr ret = operator+(a,b);
    
    a.get_ptr()->make_temporary(false);
    b.get_ptr()->make_temporary(false);
    
    return ret;
};

sym_arrow::expr sym_arrow::operator+(const value& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = operator+(a,b);

    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator+(expr&& a0, const value& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);

    expr ret = operator+(a,b);

    a.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator+(const expr& a, const value& b)
{
    expr ret;
    details::do_plus().make(ret, a, b);
    return ret;
};

sym_arrow::expr sym_arrow::operator+(const value& a, const expr& b)
{
    expr ret;
    details::do_plus().make(ret, a, b);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(const expr& a, const expr& b)
{
    expr ret;
    details::do_minus().make(ret, a,b);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(const expr& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = operator-(a,b);
    
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(expr&& a0, const expr& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    expr ret = operator-(a,b);
    a.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(expr&& a0, expr&& b0)
{
    expr a(std::move(a0));
    expr b(std::move(b0));

    a.get_ptr()->make_temporary(true);
    b.get_ptr()->make_temporary(true);

    expr ret = operator-(a,b);

    a.get_ptr()->make_temporary(false);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(const value& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = operator-(a,b);

    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator-(expr&& a0, const value& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);

    expr ret = operator-(a,b);

    a.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator-(const expr& a, const value& b)
{
    expr ret;
    details::do_minus().make(ret, a,b);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(const value& a, const expr& b)
{
    expr ret;
    details::do_minus().make(ret, a, b);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(const expr& a)
{
    expr ret;
    details::do_uminus().make(ret, a);
    return ret;
};

sym_arrow::expr sym_arrow::operator-(expr&& a0)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    
    expr ret = operator-(a);
    
    a.get_ptr()->make_temporary(false);
    return ret;
};
