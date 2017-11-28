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
#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/builder/vlist_add.h"

#include "sym_arrow/ast/builder/build_item.h"
#include "sym_arrow/ast/builder/vlist_mult.h"

namespace sym_arrow { namespace ast
{

// data representing mult_build node with int powers only
template<class Item>
class mult_build_info_int
{
    private:
        // multiplication constant
        value               m_a;

        // number of items
        size_t              m_in;

        // handles to subterms (storing exp)
        const Item*         m_iex;

        // array of integer powers
        const int*          m_ipow;        

    public:
        // constructor
        mult_build_info_int(const value& a, size_t n_, const int* pow, const Item* ex)
            :m_a(a), m_in(n_), m_ipow(pow), m_iex(ex)
        {};

        // return multiplication constant
        const value&        get_const() const           { return m_a; };

        // return number of subterms
        size_t              isize() const               { return m_in; };

        // get subexpression power
        int                 get_ipow(size_t pos) const  { return m_ipow[pos]; };

        // get subexpression
        expr_handle         get_iexpr(size_t pos) const { return m_iex[pos]; };
};

// data representing mult_build node with one real power only
class mult_build_info_real
{
    private:
        // subterm
        const expr_handle*  m_rex;

        // subterm power
        const value*        m_rpow;        

    public:
        // constructor
        mult_build_info_real(const expr_handle* ex, const value* p)
            :m_rpow(p), m_rex(ex)
        {};

        // get subterm power
        const value&        get_rpow() const    { return m_rpow[0]; };

        // get subterm
        expr_handle         get_rexpr() const   { return m_rex[0]; };
};

// data representing mult_build node with exp term only
class mult_exp_build_info
{
    private:
        // exp subterm
        expr_handle         m_exp;

    public:
        // constructor
        mult_exp_build_info(expr_handle exp_ex)
            :m_exp(exp_ex)
        {};

        // get exp subterm
        expr_handle         get_exp_handle() const  { return m_exp; };
};

// data representing mult_build node with all kinds of subterms
template<class Ipow_item, class Rpow_item>
class mult_build_info
{
    private:
        // number of integer power items
        size_t              m_in;

        // number of real power items
        size_t              m_rn;

        // handles to integer power subterms
        const Ipow_item*    m_iex;

        // handles to integer power subterms
        const Rpow_item*    m_rex;

        // handle to exp subterm
        expr_handle         m_exp;

    public:
        // constructor
        mult_build_info(size_t in, const Ipow_item* iex,
                        size_t rn, const Rpow_item* rex,
                        expr_handle exp)
            : m_in(in), m_rn(rn), m_iex(iex), m_rex(rex), m_exp(exp)
        {};

        // return number of integer power subterms
        size_t              isize() const               { return m_in; };

        // return number of real power subterms
        size_t              rsize() const               { return m_rn; };

        // get subexpression power
        int                 get_ipow(size_t pos) const  { return m_iex[pos].get_value(); };

        // get subexpression
        expr_handle         get_iexpr(size_t pos) const { return m_iex[pos].get_expr_handle(); };

        // get real subexpression power
        const value&        get_rpow(size_t pos) const  { return m_rex[pos].get_value(); };

        // get real subexpression
        expr_handle         get_rexpr(size_t pos) const { return m_rex[pos].get_expr_handle(); };

        // return true if exp subexpression is present
        bool                has_exp() const             { return m_exp != nullptr; }    

        // return exp subexpression
        expr_handle         get_exp_handle() const      { return m_exp; };
};

// class representing uncannonized multiplicative expression;
// this node is not hashed; if this node is unique and temporary,
// then new items can be added inplace
class mult_build : public sym_dag::dag_item<mult_build, term_tag, false>
{
    private:
        using base_type         = sym_dag::dag_item<mult_build, term_tag, false>;
        using ilist_type        = ilist_mult;
        using rlist_type        = rlist_mult;

    private:
        // multiplication constant
        value                   m_scal;

        // array of integer power subterms
        ilist_type*             m_ilist;

        // array of real power and exp subterms
        rlist_type*             m_rlist;

        // cannonized expression if calculated
        mutable expr            m_cannonized;

    private:
        mult_build(const mult_build&) = delete;
        mult_build& operator=(const mult_build&) = delete;

    public:
        // create mult_build containing integer powers only
        template<class Item>
        explicit mult_build(const mult_build_info_int<Item>&);

        // create mult_build containing one real power only
        explicit mult_build(const mult_build_info_real&);

        // create mult_build containing exp term only
        explicit mult_build(const mult_exp_build_info&);

        // create mult_build containing integer powers, real powers
        // and exp subterm
        template<class Ipow_item, class Rpow_item>
        explicit mult_build(const mult_build_info<Ipow_item, Rpow_item>&);

        // destuctor
        ~mult_build();       

        // delay destruction of directly accessible dag nodes
        void                    release(stack_type& stack);
        
        // get multiplication constant
        const value&            get_scal() const;

        // get ilist array
        const ilist_type*       get_ilist() const;

        // get rlist array
        const rlist_type*       get_rlist() const;

        // return true if this object stores only one integer item
        // and power value assigned to this item is one
        bool                    is_simple_mult() const;

        // return unique integer power subterm; 
        // requires is_simple_build() == true
        const build_item<int>&  unique_ielem() const;

        // return result of cannonization of this node;
        // null expr is returned if this node is not cannonized yet
        const expr&             get_cannonized() const;

        // set result of cannonization of this node
        void                    set_cannonized(const expr& ex) const;

        // perform a * (x ^ pow), where x is represented by this node;
        // return expr constructed from this node
        void                    update(expr& ret, const value& a, int pow);

        // perform a * (x ^ pow) * (y ^ b2), where x is represented by
        // this node; return expr constructed from this node
        void                    update(expr& ret, const value& a, int pow, int b2, 
                                    expr_handle s2);

        // perform x ^p, where x is represented by this node; 
        // return expr constructed from this node
        void                    make_pow(expr& ret, const value& p);

    private:
        // this item is modified after construction (delete m_cannonized)
        void                    set_modified();

        // initialize ilist if required
        void                    make_ilist();

        // initialize rlist if required
        void                    make_rlist();

        // this node is raised to a power pow
        void                    make_pow(int pow);

        // this node is multiplied by a scalar s
        void                    make_scal(const value& s);                

        // add element expr ^ pow; perform different simplifications
        void                    insert_ielem(int pow, expr_handle expr);

        // add element expr ^ pow; perform different simplifications
        void                    insert_relem(const value& pow, expr_handle expr);

        // add exp item; perform different simplifications
        void                    insert_elem_exp(expr_handle expr);

        // insert exp item to the list; should be called only by insert_elem_exp
        void                    push_back_exp(expr_handle ex);

        // insert int power item to the list; should be called only by insert_ielem
        void                    ipush_back(int pow, expr_handle ex);

        // insert real power item to the list; should be called only by insert_relem
        void                    rpush_back(const value& pow, expr_handle ex);
};

};};

#include "sym_arrow/ast/builder/mult_build.inl"
