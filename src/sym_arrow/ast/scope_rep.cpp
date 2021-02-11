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

#include "scope_rep.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/ast/symbol_context_data.h"
#include "sym_arrow/nodes/scope.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                          scope_rep
//----------------------------------------------------------------------
scope_rep::scope_rep(const scope_rep_info& pi)
    : base_type(this), m_hash(pi.m_hash), m_fresh_symbol_code(0)
{
    m_parent    = scope_ptr::from_this(pi.m_parent);
    m_name      = identifier_ptr::from_this(pi.m_name);

    if (m_name->is_qualified() == true)
        throw std::runtime_error("scope name cannot be qualified");
};

scope_rep::~scope_rep()
{};

bool scope_rep::is_global() const
{
    return (m_parent.get() == nullptr);
}

bool scope_rep::equal(const scope_rep_info& pi) const
{
    if (pi.m_name != m_name.get())
        return false;

    if (pi.m_parent != m_parent.get())
        return false;

    return true;
}

size_t scope_rep::eval_hash(const scope_rep_info& pi)
{
    if (pi.m_hash != 0)
        return pi.m_hash;

    size_t seed = (size_t)pi.m_name; 

    boost::hash_combine(seed, (size_t)pi.m_parent);

    pi.m_hash = seed;
    return seed;
}

scope_ptr scope_rep::global_scope()
{
    return context_type::get().get_context_data().global_scope();
}

scope_ptr scope_rep::make_scope(const scope_rep* parent, const std::string& name)
{
    identifier id = identifier(name);

    scope_rep_info ri   = scope_rep_info(id.get_ptr().get(), parent);
    return scope_rep::make(ri);
};

scope_ptr scope_rep::make_scope(const scope_rep* parent)
{
    // make unique unqualified identifier
    // note that only this function can create child scopes for
    // parent scope; therefore we may create special identifier with
    // fresh number in this scope
    ++parent->m_fresh_symbol_code;
    identifier_ptr id       = identifier_rep::make_special(parent->m_fresh_symbol_code, nullptr);

    scope_rep_info ri   = scope_rep_info(id.get(), parent);
    return scope_rep::make(ri);
};

scope_ptr scope_rep::make_global_scope()
{
    identifier_ptr id       = identifier_rep::make_special("$global", nullptr);

    scope_rep_info ri   = scope_rep_info(id.get(), nullptr);
    return scope_rep::make(ri);
}

identifier scope_rep::get_unique_identifier() const
{
    ++m_fresh_symbol_code;
    identifier_ptr id       = identifier_rep::make_special(m_fresh_symbol_code, this);

    return identifier(id);
}

scope_ptr scope_rep::get_parent_scope() const
{
    if (is_global() == true)
        return scope_ptr::from_this(this);
    else
        return m_parent;
};

};};
