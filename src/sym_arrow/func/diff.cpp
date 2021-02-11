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
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/func/symbol_functions.h"
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/func/diff_hash.h"
#include "sym_arrow/functions/expr_functions.h"

#include "sym_arrow/error/error_formatter.h"

#include <sstream>
#include <map>

namespace sym_arrow { namespace details
{

namespace sd = sym_arrow :: details;

class do_diff_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_diff_vis>
{
    private:
        diff_context    m_diff_context;

    public:
        using tag_type  = sym_arrow::ast::term_tag;

        expr make(const ast::expr_base* ast, const symbol& sym);

    public:
        do_diff_vis(const diff_context& dc);

        template<class Node>
        expr eval(const Node* ast, const symbol& sym);

        expr eval(const ast::scalar_rep* h, const symbol& sym);
        expr eval(const ast::symbol_rep* h, const symbol& sym);
        expr eval(const ast::add_build* h, const symbol& sym);
        expr eval(const ast::mult_build* h, const symbol& sym);
        expr eval(const ast::add_rep* h, const symbol& sym);
        expr eval(const ast::mult_rep* h, const symbol& sym);
        expr eval(const ast::function_rep* h, const symbol& sym);

    private:
        expr func_derivative(const symbol& func, size_t arg, const expr* args, 
                size_t n_args, const symbol& sym);
        void error_diff_rule_not_defined(const symbol& func_name, size_t n_args, 
                size_t arg);
};

do_diff_vis::do_diff_vis(const diff_context& dc)
    :m_diff_context(dc)
{};

expr do_diff_vis::make(const ast::expr_base* ast, const symbol& sym)
{
    expr ex = visit(ast, sym);    
    return ex;
};

expr do_diff_vis::eval(const ast::scalar_rep* h, const symbol&)
{
    (void)h;
    return ast::scalar_rep::make_zero();
};

expr do_diff_vis::eval(const ast::symbol_rep* h, const symbol& sym)
{
    if (h == sym.get_ptr().get())
    {
        // symbol found - result 1
        return ast::scalar_rep::make_one();
    }
    else 
    {
        return ast::scalar_rep::make_zero();
    };
};

expr do_diff_vis::eval(const ast::add_build* h, const symbol&)
{
    (void)h;
    assertion(0,"expression not explicit");
    throw;
};

expr do_diff_vis::eval(const ast::mult_build* h, const symbol&)
{
    (void)h;
    assertion(0,"expression not explicit");
    throw;
};

expr do_diff_vis::eval(const ast::add_rep* h, const symbol& sym)
{
    ast::symbol_handle sh = sym.get_ptr().get();

    if (ast::details::has_symbol(h, sh->get_symbol_code()) == false)
        return ast::scalar_rep::make_zero();

    expr hash           = diff_hash::get().find(h, sym);

    if (hash.is_null() == false)
        return hash;

    value one           = value::make_one();

    size_t n            = h->size();

    using item          = ast::build_item<value>;
    using item_pod      = sd::pod_type<item>;

    int size_counter    = 0;
    size_t length       = n + 1;
    item_pod::destructor_type d(&size_counter);
    sd::stack_array<item_pod> sum_buff(length, &d);    

    item* sum_buff_ptr  = sum_buff.get_cast<item>();

    for (size_t j = 0; j < n; ++j)
    {
        expr deriv      = make(h->E(j), sym);
        const value& b  = h->V(j);   

        if (deriv.get_ptr()->isa<ast::scalar_rep>() == true)
        {
            if (cast_scalar(deriv).get_value().is_zero() == true)
                continue;
        };

        new (sum_buff_ptr + size_counter) item(b, std::move(deriv));
        ++size_counter;
    };

    if (h->has_log() == true)
    {
        expr deriv      = make(h->Log(), sym);
        bool is_zero    = false;

        if (deriv.get_ptr()->isa<ast::scalar_rep>() == true)
        {
            if (cast_scalar(deriv).get_value().is_zero() == true)
                is_zero = true;
        };

        if (is_zero == false)
        {
            expr log_d  = std::move(deriv) / expr(h->Log());

            new (sum_buff_ptr + size_counter) item(one, std::move(log_d));
            ++size_counter;
        };
    };

    ast::add_build_info2<item> bi(value::make_zero(), size_counter, sum_buff_ptr, nullptr);
    ast::expr_ptr diff_all = ast::add_build::make(bi);

    expr ret    = expr(std::move(diff_all));
    ret.cannonize(false);

    diff_hash::get().add(h, sym, ret);

    return ret;
};

expr do_diff_vis::eval(const ast::mult_rep* h, const symbol& sym)
{
    namespace sd = sym_arrow :: details;

    using expr_handle   = ast::expr_handle;
    using iitem         = ast::build_item_handle<int>;
    using ritem         = ast::details::value_expr<value>;
    using iitem_pod     = sd::pod_type<iitem>;
    using mult_build    = ast::mult_build_info<iitem, ritem>;

    ast::symbol_handle sh = sym.get_ptr().get();

    if (ast::details::has_symbol(h, sh->get_symbol_code()) == false)
        return ast::scalar_rep::make_zero();

    expr hash           = diff_hash::get().find(h, sym);

    if (hash.is_null() == false)
        return hash;

    expr zero   = ast::scalar_rep::make_zero();
    expr one    = ast::scalar_rep::make_one();

    size_t in           = h->isize();
    size_t rn           = h->rsize();

    expr expr_int_deriv = zero;
    
    //form P = \prod_j IE(j)^IV(j)

    sd::stack_array<iitem_pod> ipow_buff(in + 2);
    iitem* ipow_arr     = ipow_buff.get_cast<iitem>();

    for (size_t i = 0; i < in; ++i)
        ipow_arr[i]     = iitem(h->IV(i), h->IE(i));

    expr_handle exp_h   = h->has_exp() ? h->Exp() : nullptr;

    //------------------------------------------------------------------
    // for integer powers IV(i) derivative is given by the formula
    //        \sum_i IV(i) * EX * IE(i)' / IE(i)
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // for real powers RV(i) derivative is given by the formula
    //        \sum_i  RV(i) * EX * RE(i)' / RE(i)
    //------------------------------------------------------------------

    using item          = ast::build_item<value>;
    using item_pod      = sd::pod_type<item>;

    int size_counter    = 0;
    size_t length       = in + rn + 1;
    item_pod::destructor_type d(&size_counter);
    sd::stack_array<item_pod> sum_buff(length, &d);    

    item* sum_buff_ptr  = sum_buff.get_cast<item>();

    {
        // form P_i = EX * (IE(i)' / IE(i))
        // and sum IV(i) * P_i

        for (size_t i = 0; i < in; ++i)
        {
            expr_handle ex  = ipow_arr[i].m_expr;
            expr tmp_deriv  = make(ex, sym);

            if (tmp_deriv.get_ptr()->isa<ast::scalar_rep>() == true)
            {
                if (cast_scalar(tmp_deriv).get_value().is_zero() == true)
                    continue;
            };

            ipow_arr[in]    = iitem(-1, ex);
            ipow_arr[in + 1]= iitem(1, tmp_deriv.get_ptr().get());

            int k           = ipow_arr[i].m_value;

            mult_build build_info(in+2, ipow_arr, h->rsize(), h->RVE(), exp_h);

            expr deriv = expr(ast::mult_build::make(build_info));

            new (sum_buff_ptr + size_counter) item(value::make_value(k), std::move(deriv));
            ++size_counter;
        };
        
        // form P_i = EX * (RE(i)' / RE(i))
        // and sum RV(i) * P_i

        for (size_t i = 0; i < rn; ++i)
        {
            expr_handle ex  = h->RE(i);
            expr tmp_deriv  = make(ex, sym);

            if (tmp_deriv.get_ptr()->isa<ast::scalar_rep>() == true)
            {
                if (cast_scalar(tmp_deriv).get_value().is_zero() == true)
                    continue;
            };

            ipow_arr[in]    = iitem(-1, ex);
            ipow_arr[in + 1]= iitem(1, tmp_deriv.get_ptr().get());
            const value& k  = h->RV(i);

            mult_build build_info(in+2, ipow_arr, h->rsize(), h->RVE(), exp_h);
            expr deriv = expr(ast::mult_build::make(build_info));            

            new (sum_buff_ptr + size_counter) item(k, std::move(deriv));
            ++size_counter;
        };
    };

    //------------------------------------------------------------------------------
    // exp part
    //------------------------------------------------------------------------------

    if (h->has_exp())
    {
        expr tmp_deriv  = make(exp_h, sym);

        if (tmp_deriv.get_ptr()->isa<ast::scalar_rep>() == false)
        {
            ipow_arr[in] = iitem(1, tmp_deriv.get_ptr().get());

            mult_build build_info(in+1, ipow_arr, h->rsize(), h->RVE(), exp_h);
            expr deriv = expr(ast::mult_build::make(build_info));            

            new (sum_buff_ptr + size_counter) item(value::make_one(), std::move(deriv));
            ++size_counter;
        }
        else
        {
            value v     = cast_scalar(tmp_deriv).get_value();

            if (v.is_zero() == false)
            {
                mult_build build_info(in, ipow_arr, h->rsize(), h->RVE(), exp_h);
                expr deriv = expr(ast::mult_build::make(build_info));            

                new (sum_buff_ptr + size_counter) item(v, std::move(deriv));
                ++size_counter;
            };
        }
    };

    ast::add_build_info2<item> bi(value::make_zero(), size_counter, sum_buff_ptr, nullptr);
    ast::expr_ptr diff_all = ast::add_build::make(bi);

    expr ret    = expr(std::move(diff_all));
    ret.cannonize(false);

    diff_hash::get().add(h, sym, ret);

    return ret;
};

expr do_diff_vis::eval(const ast::function_rep* h, const symbol& sym)
{
    ast::symbol_handle sh   = sym.get_ptr().get();
    size_t n                = h->size();

    if (ast::details::has_symbol(h, sh->get_symbol_code()) == false|| n == 0)
        return ast::scalar_rep::make_zero();

    expr hash           = diff_hash::get().find(h, sym);

    if (hash.is_null() == false)
        return hash;

    int size_counter        = 0;
    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    // build arguments
    for (size_t j = 0; j < n; ++j)
    {
        expr tmp            = expr(h->arg(j));

        new(buff_ptr + size_counter) expr(std::move(tmp));
        ++size_counter;
    };

    //build symbol
    symbol func = symbol(ast::symbol_ptr::from_this(h->name()));

    using item          = ast::build_item<value>;
    using item_pod      = sd::pod_type<item>;

    int sum_counter     = 0;
    item_pod::destructor_type d_sum(&sum_counter);
    sd::stack_array<item_pod> sum_buff(n, &d_sum);
    item* sum_buff_ptr  = sum_buff.get_cast<item>();

    value one           = value::make_one();

    // eval partial derivatives
    for (size_t j = 0; j < n; ++j)
    {
        expr deriv      = func_derivative(func, j, buff_ptr, n, sym);
        bool is_zero    = false;

        if (deriv.get_ptr()->isa<ast::scalar_rep>() == true)
        {
            if (cast_scalar(deriv).get_value().is_zero() == true)
                is_zero = true;
        };

        if (is_zero == false)
        {
            new (sum_buff_ptr + sum_counter) item(one, std::move(deriv));
            ++sum_counter;
        }
    };

    ast::add_build_info2<item> bi(value::make_zero(), sum_counter, sum_buff_ptr, nullptr);
    ast::expr_ptr diff_all = ast::add_build::make(bi);

    expr ret    = expr(std::move(diff_all));
    ret.cannonize(false);

    diff_hash::get().add(h, sym, ret);

    return ret;
};

expr do_diff_vis::func_derivative(const symbol& func_name, size_t arg, const expr* args, 
                                  size_t n_args, const symbol& sym)
{
    expr arg_dif    = visit(args[arg].get_ptr().get(), sym);

    bool is_zero    = false;

    if (arg_dif.get_ptr()->isa<ast::scalar_rep>() == true)
    {
        if (cast_scalar(arg_dif).get_value().is_zero() == true)
            is_zero = true;
    };

    if (is_zero == true)
        return arg_dif;

    expr dif    = m_diff_context.diff(func_name, arg, args, n_args);

    if (dif.is_null() == true)
        error_diff_rule_not_defined(func_name, n_args, arg);

    return std::move(dif) * std::move(arg_dif);
};

void do_diff_vis::error_diff_rule_not_defined(const symbol& func_name, size_t n_args, 
        size_t arg)
{
    error::error_formatter ef;
    ef.head() << "differentiation rule not defined";

    ef.new_info();
    ef.line() << "unable to find differentiation rule d/dx" << arg + 1 << " ";
        disp(ef.line(), func_name, false);

    if (n_args == 0)
        ef.line() << "[]";
    else if (n_args == 1)
        ef.line() << "[x1]";
    else if (n_args == 2)
        ef.line() << "[x1, x2]";
    else
        ef.line() << "[x1, ... x" << n_args << "]";

    throw std::runtime_error(ef.str());
};

}};

namespace sym_arrow
{

expr sym_arrow::diff(const expr& ex, const symbol& sym, const diff_context& dc)
{
    ex.cannonize(false);

    const ast::expr_base* h = ex.get_ptr().get();

    expr ret    = details::do_diff_vis(dc).make(h, sym);
    return ret;
};

expr sym_arrow::diff(const expr& ex, const symbol& sym, int n, const diff_context& dc)
{
    ex.cannonize(false);

    expr res    = ex;

    for (int i = 0; i < n; ++i)
        res     = diff(res, sym, dc);

    return res;
};

};