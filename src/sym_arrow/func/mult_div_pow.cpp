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

#include "sym_arrow/config.h"
#include "sym_arrow/nodes/expr.h"
#include "dag/dag.h"
#include "process_scalar.h"
#include "compound.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace details
{

struct do_mult : public process_scalar2<do_mult>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, const value& b)
    {
        ret = expr(a * b);
    }

    static void eval(expr& ret, const value& a, ast::expr_handle b)
    {
        return compound::plus_build(ret, value::make_zero(), a, b);
    }

    static void eval(expr& ret, ast::expr_handle a, const value& b)
    {
        return compound::plus_build(ret, value::make_zero(), b, a);
    }

    static void eval(expr& ret, ast::expr_handle a, ast::expr_handle b)
    {
        return compound::mult_build(ret, a, 1, b, 1);
    }
};

struct do_div : public process_scalar2<do_div>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, const value& b)
    {
        ret = expr(a / b);
    }

    static void eval(expr& ret, const value& a, ast::expr_handle b)
    {
        return compound::mult_build(ret, a, b, -1);
    }

    static void eval(expr& ret, ast::expr_handle a, const value& b)
    {
        return compound::plus_build(ret, value::make_zero(), inv(b), a);
    }

    static void eval(expr& ret, ast::expr_handle a, ast::expr_handle b)
    {
        return compound::mult_build(ret, a, 1, b, -1);
    }
};

struct do_pow_real : public process_scalar2<do_pow_real>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, const value& b)
    {
        ret = expr(power_real(a, b));
    }

    static void eval(expr& ret, ast::expr_handle a, const value& b)
    {
        return compound::mult_build(ret, expr(a), b);
    }

    static void eval(expr& ret, const value& a, ast::expr_handle b)
    {
        return compound::mult_build(ret, a, b);
    }

    // b should be cannonized
    static void eval(expr& ret, ast::expr_handle a, ast::expr_handle b)
    {
        return compound::mult_build(ret, expr(a), expr(b));
    }
};

struct do_pow_int : public process_scalar1<do_pow_int>
{
    using tag_type      = sym_arrow::ast::term_tag;
    using scalar_handle = const ast::scalar_rep*;
    using expr_handle   = const ast::expr_base*;

    static void eval(expr& ret, const value& a, int b)
    {
        ret = expr(power_int(a, b));
    }

    static void eval(expr& ret, ast::expr_handle a, int b)
    {
        return compound::mult_build(ret, value::make_one(), a, b);
    };
};

}};

sym_arrow::expr sym_arrow::operator*(const expr& a, const expr& b)
{
    expr tmp;
    details::do_mult().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::operator*(const expr& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);
    expr ret = operator*(a,b);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator*(expr&& a0, const expr& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    expr ret = operator*(a,b);
    a.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator*(expr&& a0, expr&& b0)
{
    expr a(std::move(a0));
    expr b(std::move(b0));

    a.get_ptr()->make_temporary(true);
    b.get_ptr()->make_temporary(true);

    expr ret = operator*(a,b);

    a.get_ptr()->make_temporary(false);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator*(const value& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = operator*(a,b);

    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator*(expr&& a0, const value& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);

    expr ret = operator*(a,b);

    a.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator*(const expr& a, const value& b)
{
    expr tmp;
    details::do_mult().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::operator*(const value& a, const expr& b)
{
    expr tmp;
    details::do_mult().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::operator/(const expr& a, const expr& b)
{
    expr tmp;
    details::do_div().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::operator/(const expr& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);
    expr ret = operator/(a,b);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator/(expr&& a0, const expr& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    expr ret = operator/(a,b);
    a.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::operator/(expr&& a0, expr&& b0)
{
    expr a(std::move(a0));
    expr b(std::move(b0));

    a.get_ptr()->make_temporary(true);
    b.get_ptr()->make_temporary(true);

    expr ret = operator/(a,b);

    a.get_ptr()->make_temporary(false);
    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator/(const value& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = operator/(a,b);

    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator/(expr&& a0, const value& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);

    expr ret = operator/(a,b);

    a.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::operator/(const expr& a, const value& b)
{
    expr tmp;
    details::do_div().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::operator/(const value& a, const expr& b)
{
    expr tmp;
    details::do_div().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::power_real(const expr& a, const expr& b)
{
    expr tmp;
    b.cannonize(do_cse_default);
    details::do_pow_real().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::power_real(expr&& a0, const expr& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);
    expr ret = power_real(a,b);
    a.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::power_real(expr&& a0, expr&& b0)
{
    expr a(std::move(a0));
    expr b(std::move(b0));

    a.get_ptr()->make_temporary(true);
    b.get_ptr()->make_temporary(true);

    expr ret = power_real(a,b);
    
    a.get_ptr()->make_temporary(false);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::power_real(const expr& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);
    expr ret = power_real(a,b);
    b.get_ptr()->make_temporary(false);
    return ret;
};

sym_arrow::expr sym_arrow::power_real(const value& a, expr&& b0)
{
    expr b(std::move(b0));

    b.get_ptr()->make_temporary(true);

    expr ret = power_real(a,b);

    b.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::power_real(expr&& a0, const value& b)
{
    expr a(std::move(a0));

    a.get_ptr()->make_temporary(true);

    expr ret = power_real(a,b);

    a.get_ptr()->make_temporary(false);

    return ret;
};

sym_arrow::expr sym_arrow::power_real(const expr& a, const value& b)
{
    expr tmp;
    details::do_pow_real().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::power_real(const value& a, const expr& b)
{
    expr tmp;
    details::do_pow_real().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::power_int(const expr& a, int b)
{
    if (b == 1)
        return a;

    expr tmp;
    details::do_pow_int().make(tmp, a, b);
    return tmp;
};

sym_arrow::expr sym_arrow::power_int(expr&& a0, int b)
{
    expr a(std::move(a0));

    if (b == 1)
        return a;

    a.get_ptr()->make_temporary(true);
    expr ret = power_int(a,b);
    a.get_ptr()->make_temporary(false);
    return ret;
};
