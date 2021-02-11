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

#include "symbol_context_data.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/func/contexts_impl.h"
#include "sym_arrow/sema/symbol_table_impl.h"

namespace sym_arrow { namespace ast { namespace details
{

symbol_context_data::~symbol_context_data()
{};

void symbol_context_data::initialize()
{
    m_reg_symbols   = registered_symbols::get();
    m_sym_tab       = sd::sym_table_impl::get();

    m_default_type  = type(identifier("real"), false);
    m_global_scope  = scope_rep::make_global_scope();

    m_sym_tab->define_type(m_default_type.type_name());
};

void symbol_context_data::close()
{
    m_default_type  = type();
    m_global_scope  = scope_ptr();

    m_sym_tab->close();
    m_reg_symbols->close();   
};

}}};
