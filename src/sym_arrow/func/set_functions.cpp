/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017-2019
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

#include "sym_arrow/functions/set_functions.h"
#include "sym_arrow/nodes/set.h"
#include "sym_arrow/sema/symbol_table_impl.h"
#include "sym_arrow/error/sema_error.h"

namespace sym_arrow
{

namespace sd    = sym_arrow::details;

bool sym_arrow::set_is_equal(const identifier& x, const identifier& y)
{
    //1. x and y must define sets

    set s1, s2;

    bool is_set1            = sd::sym_table_impl::get()->get_set_definition(x, s1);
    bool is_set2            = sd::sym_table_impl::get()->get_set_definition(y, s2);

    if (is_set1 == false)
        error::sema_error().set_not_defined(x);

    if (is_set2 == false)
        error::sema_error().set_not_defined(y);

    return s1 == s2;
}

bool sym_arrow::set_is_member(const identifier& A, const expr& x)
{
    //1. A must define set
    set s1;
    bool is_set1            = sd::sym_table_impl::get()->get_set_definition(A, s1);

    if (is_set1 == false)
        error::sema_error().set_not_defined(A);

    return s1.is_member(x);
}

bool sym_arrow::operator==(const set& s1, const set& s2)
{
    return s1.get_ptr().get() == s2.get_ptr().get();
}

bool sym_arrow::operator!=(const set& s1, const set& s2)
{
    return s1.get_ptr().get() != s2.get_ptr().get();
}

bool sym_arrow::operator<(const set& s1, const set& s2)
{
    return s1.get_ptr().get() < s2.get_ptr().get();
}

bool sym_arrow::operator>(const set& s1, const set& s2)
{
    return s1.get_ptr().get() > s2.get_ptr().get();
}

bool sym_arrow::operator<=(const set& s1, const set& s2)
{
    return s1.get_ptr().get() <= s2.get_ptr().get();
}

bool sym_arrow::operator>=(const set& s1, const set& s2)
{
    return s1.get_ptr().get() >= s2.get_ptr().get();
}


};

