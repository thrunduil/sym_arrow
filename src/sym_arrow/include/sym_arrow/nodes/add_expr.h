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

// class representing additive expression in the form:
//
//         v_0 + sum_{0 ... n-1} v_i * f_i + log|f_n|
//
// where v_0, v_i are values, and f_i, f_n are terms;
// this class cannot be constructed directly
class SYM_ARROW_EXPORT add_expr
{
    private:
        expr            m_expr;

    public:
        // default constructor creating uninitialized object
        add_expr();

        // copy and move constructors
        add_expr(const add_expr& other);
        add_expr(add_expr&& other);
        
        // assignment and move assignment
        add_expr& operator=(const add_expr& other);
        add_expr& operator=(add_expr&& other);

        // destructor
        ~add_expr();

        // return true if this object is uninitialized
        bool                is_null() const;

        // convert this object to expression
        const expr&         get_expr() const;

    public:
        // return number of subterms
        size_t              size() const;

        // return true if log term is present
        bool                has_log() const;

        // return log subterm; empty expression is returned
        // if has_log() is false
        expr                Log() const;

        // return i-th subterm
        expr                E(size_t i) const;

        // return value at i-th subterm
        const value&        V(size_t i) const;

        // return v_0 value
        const value&        V0() const;

    public: 
        // internal use only
        add_expr(const ast::add_rep* h);
};

};
