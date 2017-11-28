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
#include "dbs/dbs.h"

namespace sym_arrow { namespace ast { namespace details
{

// return true in an expression contains a symbol with given code
bool has_symbol(expr_handle h, size_t symbol_code);

// return true in an expression contains at least one symbol from the set
bool has_any_symbol(expr_handle h, const dbs_lib::dbs& set);

// return true in an expression contains all symbols from the set
bool has_all_symbols(expr_handle h, const dbs_lib::dbs& set);

// return number of different symbols in expression
size_t get_number_symbols(expr_handle h);

// add symbols in expression h to given set
void add_symbols(expr_handle h, dbs_lib::dbs& set);   

class SYM_ARROW_EXPORT expr_complexity
{
    public:
        size_t  m_subnodes;
        size_t  m_add_subnodes;
        size_t  m_mult_subnodes;        
        size_t  m_function_subnodes;
        size_t  m_scalar_subnodes;
        size_t  m_symbol_subnodes;

        size_t  m_add_children;
        size_t  m_mult_children_imult;
        size_t  m_mult_children_rmult;
        size_t  m_func_children;
        size_t  m_log_subnodes;
        size_t  m_exp_subnodes;

    public:
        expr_complexity();

        void    disp(std::ostream& os = std::cout);

        void    add_scalar();
        void    add_symbol();
        void    add_add_rep(size_t size, bool has_log);
        void    add_mult_rep(size_t isize, size_t rsize, bool has_exp);
        void    add_function_rep(size_t size);

        bool    operator<(const expr_complexity& other) const;
};

// return number of different symbols in expression
SYM_ARROW_EXPORT void measure_complexity(expr_handle h, expr_complexity& compl);

}}};