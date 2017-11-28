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

// data representing mult_rep node
template<class Iitem_type, class Ritem_type>
struct mult_rep_info
{
    // handle to exp subterm
    Iitem_type*     exp_expr;

    // number of integer power subterms
    size_t          in;

    // handles to integer power subterms
    Iitem_type*     iexpr;

    // number of real power subterms
    size_t          rn;

    // handles to real power subterms
    Ritem_type*     rexpr;

    // hash value; will be set later
    mutable size_t  m_hash_mult;    

    // constructor; data must satisfy cannonization requirement
    // of mult_rep
    mult_rep_info(size_t in_, Iitem_type* iex_, Iitem_type* exp_expr_, 
                   size_t rn_, Ritem_type* rex_)
        : in(in_), iexpr(iex_), m_hash_mult(0), exp_expr(exp_expr_), rn(rn_)
        , rexpr(rex_)
    {};
};

// dag_item representing multiplicative term; such term is represented as:
// exp|x0| x prod_{i, in} (ix_i)^pi x prod_{i,rn} |rx_i|^(ri),
// where pi is an integer, in is a number of integer power subterms,
// ri is a value, and rn is a number of real power subterms
class mult_rep : public expr_symbols<mult_rep>
{
    private:
        using base_type     = expr_symbols<mult_rep>;
        using value_expr    = details::value_expr<value>;
        using int_expr      = details::value_expr<int>;
        
    private:
        size_t              m_hash;

        size_t              m_int_size;
        size_t              m_real_size;

        int_expr*           m_int_data;
        value_expr*         m_real_data;

    private:
        mult_rep(const mult_rep&) = delete;
        mult_rep& operator=(const mult_rep&) = delete;

    public:
        // construct from a data represented by mult_rep_info class
        template<class Iitem_type, class Ritem_type>
        explicit mult_rep(const mult_rep_info<Iitem_type, Ritem_type>&);

        // destructor
        ~mult_rep();

        // test for equality
        template<class Iitem_class, class Ritem_type>
        bool                equal(const mult_rep_info<Iitem_class, Ritem_type>& pi) const;

        // evaluate hash function
        template<class Iitem_class, class Ritem_type>
        static size_t       eval_hash(const mult_rep_info<Iitem_class, Ritem_type>& pi);

        // return hash value
        size_t              hash_value() const;

        // delay destruction of directly accessible dag items
        void                release(stack_type& st);

    public:
        // return true if exp term is present
        bool                has_exp() const;

        // return exp term; requires: has_exp() == true
        expr_handle         Exp() const;

        // return number of integer power subterms
        size_t              isize() const;

        // return i-th integer power subterm
        expr_handle         IE(size_t i) const;

        // return integer power for i-th integer subterm
        int                 IV(size_t i) const;

        // return number of real power subterms
        size_t              rsize() const;

        // return i-th real power subterm
        expr_handle         RE(size_t i) const;

        // return a handle to i-th real power subterm 
        const value&        RV(size_t i) const;

        // return array of value-expression pairs for integer
        // powers
        const int_expr*     IVE() const;

        // return array of value-expression pairs for real
        // powers
        const value_expr*   RVE() const;

    public:
        // form expression obtained by removing exp term
        static void         remove_exp(const mult_rep* h, expr& res);
};

};};

#include "sym_arrow/ast/mult_rep.inl"
