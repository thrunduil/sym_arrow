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
#include "sym_arrow/ast/cannonization/cannonize.h"
#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/functions/expr_functions.h"

#include <sstream>
#include <set>

namespace sym_arrow { namespace details
{

namespace sd = sym_arrow :: details;

class do_check_rep_vis : public sym_dag::dag_visitor<sym_arrow::ast::term_tag, 
                                                        do_check_rep_vis>
{
    public:
        using tag_type  = sym_arrow::ast::term_tag;

    public:
        template<class Node>
        bool eval(const Node* ast);

        bool eval(const ast::scalar_rep* h);
        bool eval(const ast::symbol_rep* h);
        bool eval(const ast::add_build* h);
        bool eval(const ast::mult_build* h);
        bool eval(const ast::add_rep* h);
        bool eval(const ast::mult_rep* h);
        bool eval(const ast::function_rep* h);

    private:
        bool atom_add_check_rep_values(const ast::add_rep* h);
        bool atom_add_check_rep_expr(const ast::add_rep* h);
        bool atom_add_check_rep_log_expr(const ast::add_rep* h);
        bool atom_mult_check_rep_values(const ast::mult_rep* h);
        bool atom_mult_check_rep_expr(const ast::mult_rep* h);
        bool atom_mult_check_rep_exp_expr(const ast::mult_rep* h);

        bool is_cannonized(ast::expr_handle h);
        bool is_normalized(const ast::add_rep* h);
        bool is_valid_scalar(ast::expr_handle h);
        bool is_valid_value(const value& v);

        bool is_add_free(ast::expr_handle h);
        bool is_atom(ast::expr_handle h);
        bool is_simple(const ast::add_rep* h);
};

bool do_check_rep_vis::eval(const ast::scalar_rep* h)
{
    (void)h;
    return true;
};

bool do_check_rep_vis::eval(const ast::symbol_rep* h)
{
    (void)h;
    return true;
};

bool do_check_rep_vis::eval(const ast::add_build* h)
{
    //this funtion is also called on uncannonized expressions
    (void)h; 
    return true; 
};

bool do_check_rep_vis::eval(const ast::mult_build* h)
{
    //this funtion is also called on uncannonized expressions
    (void)h; 
    return true; 
};

bool do_check_rep_vis::eval(const ast::add_rep* h)
{
    size_t n_elem = h->size();
   
    if (n_elem == 0 && h->has_log() == false)
        return false;

    bool isv = atom_add_check_rep_values(h);
    if (isv == false)
        return false;

    isv = atom_add_check_rep_expr(h);
    if (isv == false)
        return false;

    if (h->has_log() == true)
    {
        isv = atom_add_check_rep_log_expr(h);
        if (isv == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::eval(const ast::mult_rep* h)
{
    if (h->isize() == 0 && h->rsize() == 0 && h->has_exp() == false)
        return false;

    bool isv = atom_mult_check_rep_values(h);
    if (isv == false)
        return false;

    isv = atom_mult_check_rep_expr(h);
    if (isv == false)
        return false;

    if (h->has_exp() == true)
    {
        isv = atom_mult_check_rep_exp_expr(h);
        if (isv == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::eval(const ast::function_rep* h)
{
    size_t n_elem = h->size();

    for (size_t i = 0; i < n_elem; ++i)
    {
        ast::expr_handle e = h->arg(i);

        if (!e)
            return false;

        if (is_cannonized(e) == false)
            return false;

        if (is_valid_scalar(e) == false)
            return false;

        if (visit(e) == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::atom_add_check_rep_values(const ast::add_rep* h)
{
    size_t size = h->size();

    for(size_t i = 0; i < size; ++i)
    {
        if (h->V(i).is_zero())
            return false;

        if (is_valid_value(h->V(i)) == false)
            return false;
    };

    if (size == 1 && h->has_log() == false)
    {
        if (h->V0().is_zero() && h->V(0).is_one())
            return false;
    };

    if (is_valid_value(h->V0()) == false)
        return false;

    return true;
};

bool do_check_rep_vis::atom_add_check_rep_expr(const ast::add_rep* h)
{
    size_t n_elem = h->size();

    for (size_t i = 0; i < n_elem; ++i)
    {
        ast::expr_handle e = h->E(i);

        if (!e)
            return false;

        if (is_cannonized(e) == false)
            return false;

        bool isv = is_atom(e) || (e->isa<ast::mult_rep>() == true);

        if (isv == false)
            return false;

        bool isv2 = (visit(e) == true);

        if (isv2 == false)
            return false;
    };

    //check sort
    if (n_elem == 0)
    {
        if (h->has_log() == true)
            return true;
        else
            return false;
    };

    for (size_t i = 1; i < n_elem; ++i)
    {
        ast::expr_handle last    = h->E(i-1);
        ast::expr_handle current = h->E(i);

        bool cmp = last < current;

        if (cmp == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::atom_add_check_rep_log_expr(const ast::add_rep* h)
{
    if (visit(h->Log()) == false)
        return false;

    if (h->Log()->isa<ast::scalar_rep>() == true)
        return false;

    if (is_cannonized(h->Log()) == false)
        return false;

    if (h->Log()->isa<ast::mult_rep>() == true)
    {
        const ast::mult_rep* mh = h->Log()->static_cast_to<ast::mult_rep>();
        if (mh->has_exp() == true)
            return false;
    };

    if (h->Log()->isa<ast::add_rep>() == true)
    {
        const ast::add_rep* ah = h->Log()->static_cast_to<ast::add_rep>();
        if (ast::cannonize::is_simple_add(ah) == true)
            return false;
    };

    return true;
};

bool do_check_rep_vis::atom_mult_check_rep_values(const ast::mult_rep* h)
{
    for (size_t i = 0; i < h->isize(); ++i)
    {
        if (h->IV(i) == 0)
            return false;
    };

    for(size_t i = 0; i < h->rsize(); ++i)
    {
        if (h->RV(i).is_zero() == true)
            return false;

        if (is_valid_value(h->RV(i)) == false)
            return false;
    };

    if (h->isize() == 1 && h->rsize() == 0)
    {
        if (h->IV(0) == 1 && h->has_exp() == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::atom_mult_check_rep_expr(const ast::mult_rep* h)
{
    size_t n_int_elem  = h->isize();
    size_t n_real_elem = h->rsize();

    for (size_t i = 0; i < n_int_elem; ++i)
    {
        ast::expr_handle e = h->IE(i);

        if (is_cannonized(e) == false)
            return false;

        if (is_atom(e) == true)
            continue;

        if (e->isa<ast::add_rep>() == false)
            return false;

        //E(i) is additive
        if (is_simple(e->static_cast_to<ast::add_rep>()) == true)
            return false;

        if (is_normalized(e->static_cast_to<ast::add_rep>()) == false)
            return false;

        if (visit(e) == false)
            return false;
    };

    for (size_t i = 0; i < n_real_elem; ++i)
    {
        ast::expr_handle e  = h->RE(i);

        if (is_cannonized(e) == false)
            return false;

        if (is_atom(e) == true)
            continue;

        if (e->isa<ast::add_rep>() == false)
            return false;

        //E(i) is additive
        if (is_simple(e->static_cast_to<ast::add_rep>()) == true)
            return false;

        if (is_normalized(e->static_cast_to<ast::add_rep>()) == false)
            return false;

        if (visit(e) == false)
            return false;
    };

    //check sort
    for (size_t i = 1; i < n_int_elem; ++i)
    {
        ast::expr_handle last    = h->IE(i-1);
        ast::expr_handle current = h->IE(i);

        bool cmp = last < current;

        if (cmp == false)
            return false;
    };

    for (size_t i = 1; i < n_real_elem; ++i)
    {
        ast::expr_handle last    = h->RE(i-1);
        ast::expr_handle current = h->RE(i);

        bool cmp = last < current;

        if (cmp == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::atom_mult_check_rep_exp_expr(const ast::mult_rep* h)
{
    if (visit(h->Exp()) == false)
        return false;

    if (h->Exp()->isa<ast::scalar_rep>() == true)
        return false;

    if (is_cannonized(h->Exp()) == false)
        return false;

    if (h->Exp()->isa<ast::add_rep>() == true)
    {
        const ast::add_rep* ah = h->Exp()->static_cast_to<ast::add_rep>();
        if (ah->has_log() == true || ah->V0().is_zero() == false)
            return false;
    };

    return true;
};

bool do_check_rep_vis::is_valid_scalar(ast::expr_handle h)
{
    if (h->isa<ast::scalar_rep>() == false)
        return true;

    bool is_valid = is_valid_value(h->static_cast_to<ast::scalar_rep>()->get_data());

    if (is_valid == false)
        return false;
    else
        return true;
}

bool do_check_rep_vis::is_valid_value(const value& h)
{
    bool is_valid   = (h.is_nan() == false);

    if (is_valid == false)
        return false;
    else
        return true;
}

bool do_check_rep_vis::is_cannonized(ast::expr_handle h)
{
    return ast::cannonize::is_cannonized(h);
};

bool do_check_rep_vis::is_simple(const ast::add_rep* h)
{
    return ast::cannonize::is_simple_add(h);
};

bool do_check_rep_vis::is_atom(ast::expr_handle h)
{
    if (h->isa<ast::function_rep>() || h->isa<ast::symbol_rep>())
        return true;
    else
        return false;
};

bool do_check_rep_vis::is_add_free(ast::expr_handle h)
{
    if (h->isa<ast::add_rep>() == false)
        return true;

    const ast::add_rep* ah = h->static_cast_to<ast::add_rep>();

    if (ah->V0().is_zero() == true)
        return true;
    else
        return false;
};

bool do_check_rep_vis::is_normalized(const ast::add_rep* h)
{
    value scal = ast::cannonize::get_normalize_scaling(h->V0(), h->size(), h->VE(), 
                                        h->has_log());
    return (scal.is_one() == true);
};

}};

namespace sym_arrow
{

bool sym_arrow::check_expression(const expr& ex)
{
    const ast::expr_base* h     = ex.get_ptr().get();

    if (!h)
        return true;

    return details::do_check_rep_vis().visit(h);
};

};