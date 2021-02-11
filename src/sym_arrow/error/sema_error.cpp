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

#include "sym_arrow/error/sema_error.h"
#include "sym_arrow/error/error_formatter.h"
#include "sym_arrow/functions/expr_functions.h"

namespace sym_arrow { namespace error
{

void sema_error::null_expr()
{
    error::error_formatter ef;
    ef.head() << "expr not initialized";

    throw std::runtime_error(ef.str());
}

void sema_error::bind_array_not_set()
{
    error::error_formatter ef;
    ef.head() << "bind array is required but not initialized (substitution context)";

    throw std::runtime_error(ef.str());
}

void sema_error::invalid_bind_size(size_t size, size_t exp_size)
{
    error::error_formatter ef;
    ef.head() << "invalid size of bind array";

    ef.new_info();
    ef.line() << "expecting array of size: " << exp_size;

    ef.new_info();
    ef.line() << "supplied array has size: " << size;

    throw std::runtime_error(ef.str());
};

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

void sema_error::const_symbol_substitution(const symbol& sym)
{
    error::error_formatter ef;
    ef.head() << "invalid substitution";

    ef.new_info();
    ef.line() << "unable to substitute constant symbol: ";
        disp(ef.line(), sym, false);

    throw std::runtime_error(ef.str());
}

void sema_error::invalid_substitution_no_convertion(const symbol& sym, const identifier& ty_ex)
{
    error::error_formatter ef;
    ef.head() << "invalid substitution";

    ef.new_info();
    ef.line() << "unable to substitute symbol: ";
        disp(ef.line(), sym, false);

    ef.new_info();
    ef.line() << "with expression of type ";
        disp(ef.line(), ty_ex, false);

    throw std::runtime_error(ef.str());
}

void sema_error::undefined_symbol(const identifier& sym, size_t n_args)
{
    error::error_formatter ef;
    ef.head() << "symbol not defined";

    ef.new_info();
    ef.line() << "symbol: ";
        disp(ef.line(), sym, false);
        ef.line() << " with " << n_args << " indices is not defined";

    throw std::runtime_error(ef.str());
};

void sema_error::invalid_symbol_args(const identifier& sym, size_t n_args, 
            const std::vector<identifier>& def_args, const type& def_type)
{
    error::error_formatter ef;
    ef.head() << "invalid number of symbol indices";

    ef.new_info();
    ef.line() << "symbol: ";
        disp_symbol_def(ef.line(), sym, def_args, def_type);
        ef.line() << " requires " << def_args.size() << " indices";

    ef.new_info();
    ef.line() << "number of supplied indices: " << n_args;

    throw std::runtime_error(ef.str());
}

void sema_error::invalid_symbol_arg(const identifier& sym, size_t arg, const identifier& t_arg, 
            const std::vector<identifier>& def_args, const type& def_type)
{
    error::error_formatter ef;
    ef.head() << "invalid symbol index";

    ef.new_info();
    ef.line() << "symbol: ";
        disp_symbol_def(ef.line(), sym, def_args, def_type);
        ef.line() << " requires index of type ";
        disp(ef.line(), def_args[arg], false);
        ef.line() << " as " << arg + 1 << " index";

    ef.new_info();
    ef.line() << "supplied index has type: ";
    disp(ef.line(), t_arg, false);

    throw std::runtime_error(ef.str());
}

void sema_error::invalid_explicit_symbol_type(const identifier& sym, 
            const std::vector<identifier>& def_args, const type&  def_type,
            const type& loc_type)
{
    error::error_formatter ef;
    ef.head() << "invalid explicit symbol type";

    ef.new_info();
    ef.line() << "symbol definition: ";
        disp_symbol_def(ef.line(), sym, def_args, def_type);

    ef.new_info();
    ef.line() << "assigned type: ";
    disp(ef.line(), loc_type.type_name(), false);

    throw std::runtime_error(ef.str());
}

void sema_error::invalid_symbol_nonconst_def(const identifier& sym, 
            const std::vector<identifier>& def_args, const type&  def_type)
{
    error::error_formatter ef;
    ef.head() << "const symbol cannot be created";

    ef.new_info();
    ef.line() << "symbol definition: ";
        disp_symbol_def(ef.line(), sym, def_args, def_type);

    ef.new_info();
    ef.line() << "symbol is not defined as const";

    throw std::runtime_error(ef.str());
}

void sema_error::disp_symbol_def(std::ostream& os, const identifier& sym, 
            const std::vector<identifier>& def_args, const type&  def_type)
{
    disp(os, sym, false);

    if (def_args.size() > 0)
    {
        os << "<";

        disp(os, def_args[0], false);

        for (size_t i = 1; i < def_args.size(); ++i)
        {
            os << ", ";
            disp(os, def_args[i], false);
        }
        os << ">";
    }

    if (def_type.type_name().is_null() == false || def_type.is_const() == true)
    {
        os << " : ";
        if (def_type.is_const() == true)
            os << "const ";

        disp(os, def_type.type_name(), false);
    }
}

}}
