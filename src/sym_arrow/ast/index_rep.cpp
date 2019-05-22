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

#include "sym_arrow/ast/index_rep.h"
#include "sym_arrow/nodes/symbol.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow { namespace ast
{

index_rep::index_rep(const index_rep_info& pi)
    :base_type(this), m_hash(pi.m_hash)
    ,m_name(identifier_ptr::from_this(pi.m_name))
    ,m_set(identifier_ptr::from_this(pi.m_set))
{
    add_symbol(pi.m_name->get_base_symbol_code());
    return;
};

index_rep::~index_rep()
{
    return;
};

bool index_rep::equal(const index_rep_info& pi) const
{
    if (this->m_name.get()->get_name() != pi.m_name)
        return false;

    if (this->m_set.get() != pi.m_set)
        return false;

    return true;
};

size_t index_rep::eval_hash(const index_rep_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = (size_t)pi.m_name; 

    boost::hash_combine(seed, pi.m_set);

    pi.m_hash = seed;
    return seed;
};

void index_rep::release(stack_type& st)
{
    (void)st;
};

};};
