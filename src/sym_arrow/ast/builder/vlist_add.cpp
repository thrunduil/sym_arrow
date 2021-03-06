/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe� Kowal 2017 - 2021
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

#include "sym_arrow/config.h"
#include "sym_arrow/ast/builder/vlist_add.h"
#include "sym_arrow/ast/builder/build_item.inl"
#include "sym_arrow/ast/builder/vlist.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                        VLIST_ADD
//-------------------------------------------------------------------
vlist_add::~vlist_add()
{};

void vlist_add::make_scal(const value& scal)
{
    m_scal          = m_scal*scal;
    m_add           = m_add*scal;
    size_t size     = current_size();

    for (size_t i = 0; i < size; ++i)
        elem(i).get_value_ref() = elem(i).get_value() * scal;
};

};};
