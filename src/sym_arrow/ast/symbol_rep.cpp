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

};};
