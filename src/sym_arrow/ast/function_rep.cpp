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

#include "sym_arrow/ast/function_rep.h"
#include "sym_arrow/nodes/symbol.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow { namespace ast
{

function_rep::function_rep(const function_rep_info& pi)
    :base_type(this), m_hash(pi.m_hash)
    ,m_size(pi.m_size), m_expr(nullptr), m_name(identifier_ptr::from_this(pi.m_name))
{
    add_symbol(pi.m_name->get_identifier_code());

    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    m_expr              = (expr_ptr*)c.malloc(m_size *sizeof(expr_ptr));

    for (size_t i = 0; i < m_size; ++i)
    {
        expr_handle h   = pi.m_args[i].get_ptr().get();
        new(m_expr+i) expr_ptr(h, sym_dag::copy_t());

        add_symbols(h);
    }
};

function_rep::~function_rep()
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    for (size_t i = 0; i < m_size; ++i)
        m_expr[i].~expr_ptr();

    c.free(m_expr, m_size * sizeof(expr_ptr));
};

bool function_rep::equal(const function_rep_info& pi) const
{
    size_t elem_size = size();

    if (elem_size != pi.m_size)
        return false;

    if (this->m_name.get() != pi.m_name)
        return false;

    for (size_t i = 0; i < elem_size; ++i)
    {
        if (pi.m_args[i].get_ptr().get() != this->arg(i))
            return false;
    };

    return true;
};

size_t function_rep::eval_hash(const function_rep_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = (size_t)pi.m_name; 

    for (size_t i = 0; i < pi.m_size; ++i)
        boost::hash_combine(seed,pi.m_args[i].get_ptr().get());

    pi.m_hash = seed;
    return seed;
};

void function_rep::release(stack_type& st)
{
    for (size_t i = 0; i < m_size; ++i)
        st.push_back(m_expr[i].release());
};

};};
