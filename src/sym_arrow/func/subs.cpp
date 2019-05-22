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
#include "dag/dag.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/functions/contexts.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/func/symbol_functions.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/functions/sym_functions.h"
#include "sym_arrow/functions/set_functions.h"
#include "sym_arrow/error/sema_error.h"

#include <sstream>

namespace sym_arrow { namespace details
{

namespace sd = sym_arrow :: details;

class do_subs_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, do_subs_vis>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class Node>
        expr eval(const Node* ast, const subs_context& sub);

        expr eval(const ast::scalar_rep* h, const subs_context& sub);
        expr eval(const ast::indexed_symbol_rep* h, const subs_context& sub);
        expr eval(const ast::add_build* h, const subs_context& sub);
        expr eval(const ast::mult_build* h, const subs_context& sub);
        expr eval(const ast::add_rep* h, const subs_context& sub);
        expr eval(const ast::mult_rep* h, const subs_context& sub);
        expr eval(const ast::function_rep* h, const subs_context& sub);
        expr eval(const ast::index_rep* h, const subs_context& sub);

    private:
        expr subs_indexed_final(const ast::indexed_symbol_rep* h, const subs_context& sc);
        expr subs_index(ast::expr_handle h, const subs_context& sc);
};

expr do_subs_vis::eval(const ast::scalar_rep* h, const subs_context& sub)
{
    (void)h;
    (void)sub;
    return expr();
}

expr do_subs_vis::eval(const ast::indexed_symbol_rep* h, const subs_context& sc)
{
    if (h->size() == 0)
        return subs_indexed_final(h, sc);

    // substitute indices first

    size_t n                = h->size();
    int size_counter        = 0;
    bool any                = false;

    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    for (size_t j = 0; j < n; ++j)
    {
        expr tmp            = subs_index(h->arg(j), sc);

        if (tmp.is_null() == true)
            tmp             = expr(h->arg(j));
        else
            any             = true;

        new(buff_ptr + size_counter) expr(std::move(tmp));
        ++size_counter;
    };

    // index name cannot be substituted

    if (any == false)
        return subs_indexed_final(h, sc);

    // make substituted index
    ast::indexed_symbol_info info(h->get_name(), n, buff_ptr);

    ast::symbol_ptr h_sub = ast::indexed_symbol_rep::make(info);

    // final substitution
    expr res = subs_indexed_final(h_sub.get(), sc);

    if (res.is_null() == true)
        return symbol(h_sub);
    else
        return res;
};

expr do_subs_vis::subs_indexed_final(const ast::indexed_symbol_rep* h, const subs_context& sc)
{
    expr res2 = sc.subs(symbol(h));
    return res2;
}

expr do_subs_vis::subs_index(ast::expr_handle h, const subs_context& sc)
{
    // only symbolic index can be substituted
    if (h->isa<ast::index_rep>() == false)
        return expr();

    const ast::index_rep* i = h->static_cast_to<ast::index_rep>();
    return visit(i, sc);
}

expr do_subs_vis::eval(const ast::index_rep* h, const subs_context& sc)
{
    expr res2 = sc.subs(symbol(h->name()));

    if (res2.is_null() == true)
        return res2;

    ast::identifier_handle sh1  = h->set_name();

    //1. if result is an index, then index set must be the same
    bool is_ind                 = res2.get_ptr()->isa<ast::index_rep>();    

    if (is_ind)
    {
        const ast::index_rep* h2    = res2.get_ptr()->static_cast_to<ast::index_rep>();        
        ast::identifier_handle sh2  = h2->set_name();

        bool eq                     = set_is_equal(identifier(sh1), identifier(sh2));
        if (eq == false)
            error::sema_error().unable_subs_index_set_different(index(h), index(h2));

        return res2;
    };

    //2. otherwise it must be a member of the index set

    bool is_member                  = set_is_member(identifier(sh1), res2);

    if (is_member == false)
        error::sema_error().unable_subs_index_not_member(index(h), res2);

    return res2;
}

expr do_subs_vis::eval(const ast::add_build* h, const subs_context& sub)
{
    (void)h;
    (void)sub;
    assertion(0,"we should not be here");
    throw;
}

expr do_subs_vis::eval(const ast::mult_build* h, const subs_context& sub)
{
    (void)h;
    (void)sub;
    assertion(0,"we should not be here");
    throw;
}

expr do_subs_vis::eval(const ast::add_rep* h, const subs_context& sc)
{
    if (ast::details::has_any_symbol(h, sc.get_symbol_set()) == false)
        return expr();

    size_t n                = h->size();
    int size_counter        = 0;
    bool any                = false;

    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    for (size_t j = 0; j < n; ++j)
    {
        expr tmp            = visit(h->E(j), sc);

        if (tmp.is_null() == true)
            tmp             = expr(h->E(j));
        else
            any             = true;

        new(buff_ptr + size_counter) expr(std::move(tmp));
        ++size_counter;
    };

    if (h->has_log() == true)
    {
        expr tmp            = visit(h->Log(), sc);

        if (tmp.is_null() == true)
            tmp             = expr(h->Log());
        else
            any             = true;

        new(buff_ptr + size_counter) expr(std::move(tmp));
        ++size_counter;
    };

    if (any == false)
        return expr();

    expr subsexpr       = expr(h->V0());

    for (size_t j = 0; j < n; ++j)
    {
        const value& b  = h->V(j);
        expr tmp        = std::move(buff_ptr[j]);
        subsexpr        = std::move(subsexpr) + (b * std::move(tmp));
    };

    if (h->has_log() == true)
    {
        expr tmp        = std::move(buff_ptr[n]);
        subsexpr        = std::move(subsexpr) + log(std::move(tmp));
    };

    return subsexpr;
};

expr do_subs_vis::eval(const ast::mult_rep* h, const subs_context& sc)
{
    if (ast::details::has_any_symbol(h, sc.get_symbol_set()) == false)
        return expr();

    size_t in               = h->isize();
    int isize_counter       = 0;
    bool any                = false;

    using expr_pod          =  sd::pod_type<expr>;

    expr_pod::destructor_type id(&isize_counter);
    sd::stack_array<expr_pod> ibuff(in, &id);    
    expr* ibuff_ptr         = reinterpret_cast<expr*>(ibuff.get());

    for (size_t i = 0; i < h->isize(); ++i)
    {
        expr tmp            = visit(h->IE(i), sc);

        if (tmp.is_null() == true)
            tmp             = expr(h->IE(i));
        else
            any             = true;

        new(ibuff_ptr + isize_counter) expr(std::move(tmp));
        ++isize_counter;
    };

    size_t rn               = h->rsize();
    int rsize_counter       = 0;

    expr_pod::destructor_type rd(&rsize_counter);
    sd::stack_array<expr_pod> rbuff(rn, &rd);    
    expr* rbuff_ptr         = reinterpret_cast<expr*>(rbuff.get());

    for (size_t i = 0; i < h->rsize(); ++i)
    {
        expr tmp            = visit(h->RE(i), sc);    

        if (tmp.is_null() == true)
            tmp             = expr(h->RE(i));
        else
            any             = true;

        new(rbuff_ptr + rsize_counter) expr(std::move(tmp));
        ++rsize_counter;
    };

    expr exp_subs;

    if (h->has_exp())
    {
        exp_subs            = visit(h->Exp(), sc);

        if (exp_subs.is_null() == true)
            exp_subs        = expr(h->Exp());
        else
            any             = true;
    };

    if (any == false)
        return expr();

    expr subsexprint = ast::scalar_rep::make_one();

    for (size_t i = 0; i < h->isize(); ++i)
    {
        // integer powers IE(i)^IV(i)
        expr tmp            = std::move(ibuff_ptr[i]);

        if (h->IV(i) != 1)
            tmp             = power_int(std::move(tmp), h->IV(i));

        subsexprint         = std::move(subsexprint) * std::move(tmp);
    };

    expr subsexprreal       = ast::scalar_rep::make_one();

    for (size_t i = 0; i < h->rsize(); ++i)
    {
        // real powers RE(i)^RV(i)
        expr tmp_base       = std::move(rbuff_ptr[i]);
        const value& a      = h->RV(i);

        expr tmp            = power_real(std::move(tmp_base), a);
        subsexprreal        = std::move(subsexprreal) * std::move(tmp);
    };

    expr subsexprexp        = ast::scalar_rep::make_zero();

    if (h->has_exp())
        subsexprexp         = std::move(exp_subs);

    return std::move(subsexprint) * exp(std::move(subsexprexp)) * std::move(subsexprreal);
};

expr do_subs_vis::eval(const ast::function_rep* h, const subs_context& sc)
{
    size_t n                = h->size();

    if (ast::details::has_any_symbol(h, sc.get_symbol_set()) == false || n == 0)       
        return expr();

    // function name is not substituted

    int size_counter        = 0;
    bool any                = false;

    using expr_pod          =  sd::pod_type<expr>;
    expr_pod::destructor_type d(&size_counter);
    sd::stack_array<expr_pod> buff(n, &d);    

    expr* buff_ptr          = reinterpret_cast<expr*>(buff.get());

    for (size_t j = 0; j < n; ++j)
    {
        expr tmp            = visit(h->arg(j), sc);

        if (tmp.is_null() == true)
        {
            tmp             = expr(h->arg(j));
        }
        else
        {
            tmp.cannonize(do_cse_default);
            any             = true;
        };

        new(buff_ptr + size_counter) expr(std::move(tmp));
        ++size_counter;
    };

    if (any == false)
        return expr();

    using info              = ast::function_rep_info;
    info f_info             = info(h->name(), n, buff_ptr);

    if (f_info.are_values_valid() == false)
        return scalar::make_nan();

    {
        expr v;
        bool evaled         = global_function_evaler()
                                .eval_function(identifier(h->name()), buff_ptr, n, v);

        if (evaled == true)
            return v;
    }

    ast::expr_ptr ep        = ast::function_rep::make(f_info);
    return expr(ep);
};

}};

namespace sym_arrow
{

expr sym_arrow::subs(const expr& ex, const symbol& sym, const expr& sub)
{
    subs_context sc;
    
    sc.add_symbol(sym, sub);

    return subs(ex, sc);
};

expr sym_arrow::subs(const expr& ex, const subs_context& sub)
{
    ex.cannonize(do_cse_default);

    const ast::expr_base* h     = ex.get_ptr().get();

    expr ret = details::do_subs_vis().visit(h, sub);

    if (ret.is_null() == true)
        return ex;
    else
        return ret;
};

};