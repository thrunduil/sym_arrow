/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe� Kowal 2017
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

#include "symbol_rep.h"
#include "sym_arrow/ast/ast.h"
#include "term_context_data.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                          symbol_rep
//----------------------------------------------------------------------
symbol_rep::symbol_rep(const named_symbol_info& info)
    :base_type(this)
    , m_code(context_type::get().get_context_data().get_fresh_symbol_code())
{
    size_t N            = info.m_size;
    size_t hash         = eval_hash(info);
    m_name              = details::string_data(info.m_name, N, hash);

    context_type::get().get_context_data().register_symbol(this);
};

symbol_rep::~symbol_rep()
{
    m_name.destroy();
    context_type::get().get_context_data().unregister_symbol(this);
};

size_t symbol_rep::eval_hash(const named_symbol_info& info)
{
    size_t seed = 0;

    for (size_t i = 0; i < info.m_size; ++i)
        boost::hash_combine(seed, info.m_name[i]);

    return seed;
};

bool symbol_rep::equal(const named_symbol_info& info) const
{
    if (info.m_size != this->get_name_size())
        return false;

    int cmp = strcmp(this->get_name(), info.m_name);

    if (cmp != 0)
        return false;

    return true;
};

//----------------------------------------------------------------------
//                          indexed_symbol_rep
//----------------------------------------------------------------------

indexed_symbol_rep::indexed_symbol_rep(const indexed_symbol_info& pi)
    :base_type(this), m_hash(pi.m_hash)
    ,m_size(pi.m_size), m_expr(nullptr), m_name(symbol_ptr::from_this(pi.m_name))
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    m_expr              = (expr_ptr*)c.malloc(m_size *sizeof(expr_ptr));

    for (size_t i = 0; i < m_size; ++i)
    {
        expr_handle h   = pi.m_args[i].get_ptr().get();
        new(m_expr+i) expr_ptr(h, sym_dag::copy_t());
    }
};

indexed_symbol_rep::~indexed_symbol_rep()
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    for (size_t i = 0; i < m_size; ++i)
        m_expr[i].~expr_ptr();

    c.free(m_expr, m_size * sizeof(expr_ptr));
};

bool indexed_symbol_rep::equal(const indexed_symbol_info& pi) const
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

size_t indexed_symbol_rep::eval_hash(const indexed_symbol_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = (size_t)pi.m_name; 

    for (size_t i = 0; i < pi.m_size; ++i)
        boost::hash_combine(seed,pi.m_args[i].get_ptr().get());

    pi.m_hash = seed;
    return seed;
};

void indexed_symbol_rep::release(stack_type& st)
{
    for (size_t i = 0; i < m_size; ++i)
        st.push_back(m_expr[i].release());
};

};};
