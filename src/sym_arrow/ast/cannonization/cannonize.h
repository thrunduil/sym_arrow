/*
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe� Kowal 2017 - 2021
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

#pragma once

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/builder/build_item.h"
#include "sym_arrow/utils/temp_value.h"

namespace sym_arrow { namespace ast
{

namespace sd = sym_arrow :: details;

// transformation of the expression to the canonical form
class cannonize
{
    private:
        using aab_value             = std::pair<const add_build*, value>;
        using collect_stack_type    = std::vector<aab_value>;
        using add_item_handle       = build_item_handle<value>;
        using expr_stack            = std::vector<expr>;
        using temp_value            = sd::temp_value<value, value::is_pod>;

    private:
        expr_stack*                 m_expr_stack;
        mutable temp_value          m_temp_vals;

    private:
        cannonize(const cannonize&) = delete;
        cannonize& operator=(const cannonize&) = delete;

    public:
        cannonize();
        ~cannonize();

        // cannonize expression
        expr            make(const expr& ex, bool do_cse);

        // cannonize expression and perform common subexpression elimination
        // even for cannonized add terms
        expr            make_cse(const expr& ex);

        // cannonize mult build 
        expr            make_mult(const mult_build* h, bool do_cse);

        // cannonize add build
        expr            make_add(const add_build* h, bool do_cse);

        // cannonize decomposed add build
        expr            make_add(const value& add, size_t size, 
                            const build_item<value>* h, bool do_cse);

        // cannonize decomposed mult expr
        expr            make_mult(size_t isize, size_t rsize, bool has_exp, 
                            build_item_handle<int>* ih, build_item_handle<value>* rh, 
                            value& scal, bool do_cse);

        // add term is simple if has the form a + bx, where a = 0
        static bool     is_simple_add(const add_rep* h);

        // return true if add expression is normalized
        static bool     is_normalized(const add_rep* h);

        // normalize add expression
        expr_ptr        normalize(const add_rep* h, value& scal) const;

        // get normalization scalar; all scalars in add_rep should be multiplied
        // by this scalar in order to obtain normalized representation
        template<class Item>
        static value    get_normalize_scaling(const value& V0, size_t n, const Item* V, 
                            bool has_log);

        // get normalization scalar for add_rep expression represented as
        // h = V0 + sum_{i = 1}^n V[i-1] * v; all scalars in add_rep should be multiplied
        // by this scalar in order to obtain normalized representation
        template<class Item>
        static value    get_normalize_scaling(const value& V0, size_t n, const Item* V, 
                            bool has_log, const value& v);

        // form add + v * h, where h is add_rep expression and normalize result
        value           add_scalar_normalize(const add_rep* h, const value& add, const value& v, 
                            expr& res);

        // form add + h, where h is add_rep expression and normalize result
        value           add_scalar_normalize(const add_rep* h, const value& add, expr& res);

        // return true is expression is cannonized
        static bool     is_cannonized(const expr& ex);
        static bool     is_cannonized(expr_handle ex);

    private:
        // implements cannonization        
        expr            make_add_impl(const add_build* h,  bool do_cse);
        expr            make_add_impl(const add_rep* h);
        expr            process_add(size_t n, item_collector_add& ic, bool do_cse);

        // update collected add items after succesful factorization
        void            process_add_factorization(const expr& fact, item_collector_add& ic,
                            size_t& n, const value& scal, bool do_cse);
        
        // represent log[log_h] * v as log[res]
        expr            mult_log(expr_handle log_h, const value& v);

        void            process_log(item_collector_add& ic, bool do_cse);
        expr            make_mult_impl(const mult_build* h, bool do_cse); 
        expr            process_mult(item_collector_mult& ic, value& scal, bool do_cse);
        void            process_exp(item_collector_mult& ic, bool& has_exp, value& scal,
                            expr& ex_term, expr& log_term, bool do_cse);

        // build final add expression
        expr            finalize_add(item_collector_add& ic, size_t n);

        // calculate number of elements in build expressions
        void            calc_size(const mult_build* h, item_collector_size& ic) const;

        template<class Value_t, class Derived>
        void            calc_size(const vlist_mult<Value_t, Derived>* h, 
                            item_collector_size& ic) const;
        template<class Value_t, class Derived>
        void            calc_size(item_collector_size& ic, const build_item<Value_t>& it) const;

        size_t          calc_size_add(const vlist_add& h) const;
        size_t          calc_size_add(const build_item<value>& elem) const;
        size_t          calc_size_add(const add_build* h) const;
        
        // collect items from build expressions
        void            collect_items_root(const vlist_add& h, item_collector_add& ic, 
                            const value& scal) const;
        void            collect_items_root(size_t size, const build_item<value>* arr, 
                            item_collector_add& ic, const value& scal) const;

        void            collect_items(const vlist_add& h, item_collector_add& ic, const value& scal, 
                            collect_stack_type& stack) const;
        void            collect_items(item_collector_add& ic, const value& scal, 
                            const build_item<value>& elem, collect_stack_type& stack) const;
        void            collect_items(const add_rep* h, item_collector_add& ic, 
                            const value& scal) const;
        void            collect_items_stack(item_collector_add& ic, collect_stack_type& stack) const;

        template<class Pow_value>
        void            collect_items(const ilist_mult* il, item_collector_mult& ic, 
                            const Pow_value& pow) const;

        template<class Pow_value>
        void            collect_items(const rlist_mult* rl, item_collector_mult& ic, 
                            const Pow_value& pow) const;

        template<class Value_t, class Pow_value>
        void            collect_items(item_collector_mult& ic, const Pow_value& pow, 
                            const build_item<Value_t>& elem) const;
        template<class Pow_value>
        void            collect_items(const mult_build& h, item_collector_mult& ic, 
                            const Pow_value& pow) const;

        // create exp expression
        expr            make_exp_mult_rep(const expr& ex_exp) const;

        // protect expression from being destroyed
        void            hold_expr(const expr& ex);
};

};};

#include "cannonize.inl"