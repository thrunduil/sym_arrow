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

#pragma once

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/ast/builder/build_item.h"

namespace sym_arrow { namespace ast
{
    struct ipow_item;
    struct exp_item;

    template<class Value_type>
    class build_item_handle;

}}

namespace sym_arrow { namespace details
{

template<class Type>
struct is_effective_pod
{};

template<class Val>
struct is_effective_pod<ast::build_item_handle<Val>>
{
    static const bool value = true;
};

template<>
struct is_effective_pod<ast::ipow_item>
{
    static const bool value = true;
};

template<>
struct is_effective_pod<ast::exp_item>
{
    static const bool value = true;
};

template<>
struct is_effective_pod<value>
{
    static const bool value = value::is_pod;
};

template<class Val>
struct is_effective_pod<Val*>
{
    static const bool value = true;
};

template<>
struct is_effective_pod<int>
{
    static const bool value = true;
};

}};