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

#include "sym_arrow/sema/typing.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/ast/symbol_context_data.h"
#include "sym_arrow/sema/symbol_table_impl.h"

namespace sym_arrow { namespace details
{

bool details::is_constant(const expr& ex)
{
    (void)ex;

    //TODO:
    return false;
}

bool details::is_constant(const symbol& s)
{
    (void)s;

    //TODO:
    return false;
}

bool details::is_convertible(const identifier& from, const identifier& to)
{
    if (from == to)
        return true;
    else
        return false;
}

identifier details::get_type(const expr& ex)
{
    bool is_sym     = ex.get_ptr()->isa<ast::indexed_symbol_rep>();

    if (is_sym == false)
    {
        //TODO
        return sym_dag::dag_context<ast::unique_nodes_tag>::get().get_context_data().default_id();
    }

    symbol id       = cast_symbol(ex);

    return id.get_type();
}

}};