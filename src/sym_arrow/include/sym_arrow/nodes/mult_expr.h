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

#pragma once

#include "sym_arrow/nodes/expr.h"

namespace sym_arrow
{

// class representing multiplicative expression in the form:
//
//     exp|d| x prod_{0 ... in-1} (f_i)^{p_i} 
//            x prod_{0 ... rn-1} |g_i|^(v_i)
//
// where in is a number of integer power subterms, rn is a number 
// of real power subterms; d, f_i, g_i, are subexpressions,
// p_i is an integer, and v_i is a value;
// this class cannot be constructed directly
class SYM_ARROW_EXPORT mult_expr
{
    private:
        expr                m_expr;

    public:
        // default constructor creating uninitialized object
        mult_expr();

        // copy and move constructors
        mult_expr(const mult_expr& other);
        mult_expr(mult_expr&& other);
        
        // assignment and move assignment
        mult_expr& operator=(const mult_expr& other);
        mult_expr& operator=(mult_expr&& other);

        // destructor
        ~mult_expr();

        // return true if this object is uninitialized
        bool                is_null() const;

        // convert this object to expression
        const expr&         get_expr() const;

    public:
        // return number of integer power subterms
        size_t              isize() const;

        // return number of real power subterms
        size_t              rsize() const;

        // return true if exp term is present
        bool                has_exp() const;

        // return exp subterm; empty expression is returned
        // if has_exp() is false
        expr                Exp() const;

        // return i-th integer power subterm
        expr                IE(size_t i) const;

        // return integer power for i-th integer subterm
        int                 IV(size_t i) const;

        // return i-th real power subterm
        expr                RE(size_t i) const;

        // return i-th real power subterm (value part)
        const value&        RV(size_t i) const;

    public: 
        // internal use only
        mult_expr(const ast::mult_rep* h);
};

};
