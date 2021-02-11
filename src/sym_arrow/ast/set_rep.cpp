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

#include "set_rep.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/symbol_rep.h"

#include <algorithm>

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                          set_rep
//----------------------------------------------------------------------
set_rep::set_rep(const set_rep_info& pi)
    :base_type(this), m_hash(pi.m_hash)
    ,m_size(pi.m_size), m_elements(nullptr)
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<unique_nodes_tag>;
    context_type& c     = context_type::get();

    m_elements          = (elem_type*)c.malloc(m_size *sizeof(elem_type));

    for (size_t i = 0; i < m_size; ++i)
    {
        elem_handle h   = pi.m_args[i].get_ptr().get();
        new(m_elements+i) elem_type(h, sym_dag::copy_t());
    }

};

set_rep::~set_rep()
{
    if (m_size == 0)
        return;

    using context_type  = sym_dag::dag_context<unique_nodes_tag>;
    context_type& c     = context_type::get();

    for (size_t i = 0; i < m_size; ++i)
        m_elements[i].~elem_type();

    c.free(m_elements, m_size * sizeof(elem_type));
};

size_t set_rep::eval_hash(const set_rep_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = pi.m_args[0].get_ptr()->hash_value(); 

    for (size_t i = 1; i < pi.m_size; ++i)
        boost::hash_combine(seed,pi.m_args[i].get_ptr()->hash_value());

    pi.m_hash = seed;
    return seed;
};

bool set_rep::equal(const set_rep_info& pi) const
{
    size_t elem_size = size();

    if (elem_size != pi.m_size)
        return false;

    for (size_t i = 0; i < elem_size; ++i)
    {
        if (pi.m_args[i].get_ptr().get() != this->arg(i))
            return false;
    };

    return true;
};

bool set_rep::is_member(const expr& x) const
{
    if (x.get_ptr()->isa<symbol_rep>() == false)
        return false;

    size_t n    = this->size();

    if (n == false)
        return false;

    symbol_handle is    = x.get_ptr()->static_cast_to<symbol_rep>();    

    if (is->size() != 0)
        return false;

    identifier_handle sh    = is->get_name();
    identifier_ptr sp       = identifier_ptr::from_this(sh);

    auto pos                = std::lower_bound(m_elements, m_elements + n, sp);

    if (pos == m_elements + n)
        return false;

    return true;
};

};};
