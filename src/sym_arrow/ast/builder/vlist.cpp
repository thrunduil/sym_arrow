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

#pragma once

#include "sym_arrow/ast/builder/vlist.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                        vlist_stack
//-------------------------------------------------------------------
vlist_stack::vlist_stack()
    : m_parent_stack(nullptr)
    , m_expr_stack(expr_vec_handle::get_handle())
{};

vlist_stack::vlist_stack(expr_stack& stack)
    : m_parent_stack(&stack)
    , m_expr_stack(expr_vec_handle::get_empty_handle())
{};

vlist_stack::~vlist_stack()
{};

void vlist_stack::release(const expr& ex)
{
    this->push_back(const_cast<expr_ptr&>(ex.get_ptr()).release());
};

};};
