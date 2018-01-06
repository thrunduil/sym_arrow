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

#include "sym_arrow/ast/cannonization/cse_hash_data.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------------
//                        cse_hash_data
//----------------------------------------------------------------------

cse_hash_data::cse_hash_data()
{};

cse_hash_data::cse_hash_data(const expr& simpl)
    : m_simplified(simpl.get_ptr())
{};

expr cse_hash_data::get_simplified_expr() const
{
    return expr(m_simplified.lock());
}

bool cse_hash_data::is_empty() const
{
    return m_simplified.expired() == true;
}

void cse_hash_data::release(stack_type&) const
{};

};};
