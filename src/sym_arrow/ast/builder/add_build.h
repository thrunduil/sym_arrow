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
#include "dag/dag.h"
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/nodes/value.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/nodes/expr.h"

namespace sym_arrow { namespace ast
{

// data representing add_build node
template<class Item>
class add_build_info
{
    private:
        // free scalar
        value               m_a;

        // number of subterms
        size_t              n;

        // array of subterm scalars
        const value*        m_val;

        // array of subterms
        const Item*         m_ex;

        // log subterm if present
        const Item*         m_log;

    public:
        // constructor
        add_build_info(const value& a, size_t n_, const value* val, 
                            const Item* ex, const Item* log)
            :m_a(a), n(n_), m_val(val), m_ex(ex), m_log(log)
        {};

        // get free scalar
        const value&        get_const() const           { return m_a; };

        // number of subterms
        size_t              size() const                { return n; };

        // return true, if log subterm is present
        bool                has_log() const             { return m_log != nullptr; };

        // get scalar at i-th subterm
        const value&        get_mult(size_t i) const    { return m_val[i]; };

        // get i-th subterm
        expr_handle         get_expr(size_t i) const    { return m_ex[i]; };

        // get log subterm; requires has_log() == true
        expr_handle         get_expr_log() const        { return *m_log; };
};

// data representing add_build node with log term only
class add_log_build_info
{
    private:
        // log subterm
        expr_handle         m_ex;

    public:
        // constructor
        explicit add_log_build_info(const expr_handle& ex)
            :m_ex(ex)
        {};

        // get log subterm
        expr_handle         get_expr() const            { return m_ex; };
};

template<class Item>
class add_build_info2
{
    private:
        // free scalar
        value               m_a;

        // number of subterms
        size_t              n;

        // array of subterms with values
        const Item*         m_ex;

        // log subterm if present
        const Item*         m_log;

    public:
        // constructor
        add_build_info2(const value& a, size_t n_, const Item* ex, const Item* log)
            :m_a(a), n(n_), m_ex(ex), m_log(log)
        {};

        // get free scalar
        const value&        get_const() const           { return m_a; };

        // number of subterms
        size_t              size() const                { return n; };

        // return true, if log subterm is present
        bool                has_log() const             { return m_log != nullptr; };

        // get scalar at i-th subterm
        const value&        get_mult(size_t i) const    { return m_ex[i].get_value(); };

        // get i-th subterm
        expr_handle         get_expr(size_t i) const    { return m_ex[i].get_expr_handle(); };

        // get log subterm; requires has_log() == true
        expr_handle         get_expr_log() const        { return m_log->get_expr_handle(); };
};

// class representing uncannonized additive expression;
// this node is not hashed; if this node is unique and temporary,
// then new items can be added inplace
class add_build : public sym_dag::dag_item<add_build, term_tag, false>
{
    private:
        using base_type     = sym_dag::dag_item<add_build, term_tag, false>;
        using vlist_type    = vlist_add;

    private:
        // array of subterms
        vlist_type          m_vlist;

        // cannonized and normalized expression if calculated
        mutable expr        m_cannonized;

        // normalization constant for cannonized expression
        mutable value       m_normalization;

    public:
        // create add_build node
        template<class Item>
        explicit add_build(const add_build_info<Item>&);

        // create add_build with log term only
        explicit add_build(const add_log_build_info&);               

        // create add_build node
        template<class Item>
        explicit add_build(const add_build_info2<Item>&);

        // destructor
        ~add_build();

        // delay destruction of directly accessible dag nodes
        void                release(stack_type& stack);

        // get array of subterms
        const vlist_type&   get_vlist() const;

        // return true if this object stores only one regular item
        // and add scalar is zero
        bool                is_simple_add() const;

        // return unique subterm; 
        // requires is_simple_build() == true
        const build_item<value>&
                            unique_elem() const;

        // return cannonization and normalization constant
        const expr&         get_cannonized(value& scal) const;

        // set result of cannonization and normalization of this node
        void                set_cannonized(const expr& ex, const value& scal) const;

    public:
        // perform a + (scal * x), where x is represented by this node;
        // return expr constructed from this node
        void                update(expr& ret, const value& a, const value& scal);

        // perform a + (scal * x) + b2 * s2, where x is represented by
        // this node; return expr constructed from this node
        void                update(expr& ret, const value& a, const value& scal, 
                                const value& b2, expr_handle s2); 

    private:
        add_build(const add_build&) = delete;
        add_build& operator=(const add_build&) = delete;

    private:
        // this item is modified after construction (delete m_cannonized)
        void                set_modified();               

        // this node is multipled by val
        void                make_scal(const value& val);

        // scalar v is added to this node
        void                make_add(const value& v);

        // add log item; perform different simplifications
        void                insert_log_elem(expr_handle expr);

        // add element scal * x
        void                insert_elem(const value& scal, expr_handle expr);

        // insert log item to the list; should be called only by insert_log_elem
        void                push_back_log(expr_handle ex);

        // insert item to the list; should be called only by insert_elem
        void                push_back(const value& val, expr_handle ex);
};

};};

#include "sym_arrow/ast/builder/add_build.inl"
