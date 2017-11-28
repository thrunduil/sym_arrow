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

#include "sym_arrow/nodes/expr.h"

namespace sym_arrow
{

// class representing function_expr expression in the form:
//
//         sym[f_0, ..., f_n]
//
// where f_i is a term, and n >= 0;
// this class cannot be constructed directly
class SYM_ARROW_EXPORT function_expr
{
    private:
        expr            m_expr;

    public:
        // default constructor creating uninitialized object
        function_expr();

        // copy and move constructors
        function_expr(const function_expr& other);
        function_expr(function_expr&& other);
        
        // assignment and move assignment
        function_expr& operator=(const function_expr& other);
        function_expr& operator=(function_expr&& other);

        // destructor
        ~function_expr();

        // return true if this object is uninitialized
        bool                is_null() const;

        // convert this object to expression
        const expr&         get_expr() const;

    public:
        // return number of arguments
        size_t              size() const;

        // get i-th argument
        expr                arg(size_t i) const;

        // get function_expr name
        symbol              name() const;

    public: 
        // internal use only
        function_expr(const ast::function_rep* h);
};

};
