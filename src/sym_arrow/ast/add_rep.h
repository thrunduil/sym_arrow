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

#pragma once

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/value.h"
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/ast/ast_rep_details.h"

namespace sym_arrow { namespace ast
{

// data representing add_rep node
template<class Item_type>
struct add_rep_info
{
    // number of subterms
    size_t              n;

    // free scalar
    const value*        scal0;

    // handles to subterms (storing a value and a term)
    const Item_type*    elems;

    // handle to exp subterm
    const Item_type*    log_expr;

    // hash value; will be set later
    mutable size_t      m_hash_add;

    // constructor; data must satisfy cannonization requirement
    // of add_rep
    add_rep_info(const value* a0, size_t n_expr, const Item_type* expr, 
                    const Item_type* log_expr_)
        : scal0(a0), n(n_expr), elems(expr), log_expr(log_expr_)
        , m_hash_add(0) 
    {};

    // return true if all scalars are finite and false otherwise
    bool are_values_valid() const;
};

// class representing a0 + sum_{i,n} ai * ei + log|el|
// where a0, ai are values, and ei, el are terms
class add_rep : public expr_symbols<add_rep>
{
    private:
        using base_type     = expr_symbols<add_rep>;
        using value_expr    = details::value_expr<value>;

    private:
        size_t              m_hash;
        size_t              m_size;
        value_expr*         m_data;

    private:
        add_rep(const add_rep&) = delete;
        add_rep& operator=(const add_rep&) = delete;        

    public:
        // construct from a data represented by add_rep_info class
        template<class Item_type>
        explicit add_rep(const add_rep_info<Item_type>&);

        // destructor
        ~add_rep();                                

        // test for equality
        template<class Item_type>
        bool                equal(const add_rep_info<Item_type>& pi) const;

        // evaluate hash function
        template<class Item_type>
        static size_t       eval_hash(const add_rep_info<Item_type>& pi);

        // return hash value
        size_t              hash_value() const;

        // delay destruction of directly accessible dag items
        void                release(stack_type& st);

    public:
        // return number of subterms
        size_t              size() const;

        // return true if log term is present
        bool                has_log() const;

        // return a0 value
        const value&        V0() const;

        // return log subterm; requires has_log() == true
        expr_handle         Log() const;

        // return i-th subterm
        expr_handle         E(size_t i) const;

        // return value at i-th subterm
        const value&        V(size_t i) const;

        // return array of value-expression pairs
        const value_expr*   VE() const;

        // return value-expression pair associated with the log
        // subterm (i.e. [V0(), Log()]; requires has_log() == true
        const value_expr&   VLog() const;

        // return true, if this expression is normalized
        bool                is_normalized() const;

        // mark this object as normalized
        void                set_normalized();

    public:
        // form expression obtained by removing additive term
        // add is set to V0(), are res stores resulting expression
        static void         remove_add(const add_rep* h, value& add, expr& res);

        // form expression obtained by removing add and log term
        static void         remove_add_log(const add_rep* h, expr& res);
};

};};

#include "sym_arrow/ast/add_rep.inl"
