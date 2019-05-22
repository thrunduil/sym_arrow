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

#include "sym_arrow/nodes/index.inl"
#include "sym_arrow/ast/index_rep.h"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/functions/expr_functions.h"
#include "sym_arrow/utils/stack_array.h"
#include "sym_arrow/sema/symbol_table.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow 
{

namespace sd = sym_arrow::details;

index::~index()
{};

index::index(const identifier& sym, const identifier& set)
    :m_expr()
{
    //1. set name must be defined
    bool is_def = sym_table::get().is_defined_set(set);

    if (is_def == false)
        error::sema_error().set_not_defined(set);
    
    m_expr = ast::index_rep::make(ast::index_rep_info(sym.get_ptr().get(), 
                                            set.get_ptr().get()));
};

symbol index::get_name() const
{
    return symbol(get_ptr()->name());
}

identifier index::set_name() const
{
    return identifier(get_ptr()->set_name());
}

};