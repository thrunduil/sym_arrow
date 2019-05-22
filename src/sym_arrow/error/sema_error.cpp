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

#include "sym_arrow/error/sema_error.h"
#include "sym_arrow/error/error_formatter.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace error
{

void sema_error::set_not_defined(const identifier& set)
{
    error::error_formatter ef;
    ef.head() << "invalid set name ";
    disp(ef.head(), set, false);

    ef.new_info();
    ef.line() << "set is not defined";

    throw std::runtime_error(ef.str());
};

void sema_error::type_not_defined(const identifier& set)
{
    error::error_formatter ef;
    ef.head() << "undefined type name ";
    disp(ef.head(), set, false);

    throw std::runtime_error(ef.str());
};

void sema_error::set_already_defined(const identifier& set)
{
    error::error_formatter ef;
    ef.head() << "invalid set name ";
    disp(ef.head(), set, false);

    ef.new_info();
    ef.line() << "symbol is already defined";

    throw std::runtime_error(ef.str());
};

void sema_error::type_already_defined(const identifier& set)
{
    error::error_formatter ef;
    ef.head() << "invalid type name ";
    disp(ef.head(), set, false);

    ef.new_info();
    ef.line() << "type is already defined";

    throw std::runtime_error(ef.str());
};

void sema_error::symbol_already_defined(const identifier& set)
{
    error::error_formatter ef;
    ef.head() << "invalid symbol name ";
    disp(ef.head(), set, false);

    ef.new_info();
    ef.line() << "symbol is already defined";

    throw std::runtime_error(ef.str());
};

void sema_error::set_elem_multiply_defined(const identifier& set_elem)
{
    error::error_formatter ef;
    ef.head() << "invalid set element ";
    disp(ef.head(), set_elem, false);

    ef.new_info();
    ef.line() << "element is already defined in the set";

    throw std::runtime_error(ef.str());
};

void sema_error::unable_subs_index_set_different(const index& i1, const index& i2)
{
    error::error_formatter ef;
    ef.head() << "invalid substitution of an index by an index";

    ef.new_info();
    ef.line() << "unable to substitute index ";
        disp(ef.line(), i1, false);
        ef.line() << " by index ";
        disp(ef.line(), i2, false);

    ef.new_info();
    ef.line() << "index' sets are different";

    throw std::runtime_error(ef.str());
}

void sema_error::unable_subs_index_not_member(const index& i1, const expr& member)
{
    error::error_formatter ef;
    ef.head() << "invalid substitution of an index by a set member";

    ef.new_info();
    ef.line() << "unable to substitute index ";
        disp(ef.line(), i1, false);
        ef.line() << " by ";
        disp(ef.line(), member, false);

    ef.new_info();
    ef.line() << "result of substitution is not a member of ";
        disp(ef.line(), i1.set_name(), false);

    throw std::runtime_error(ef.str());
}


}}
