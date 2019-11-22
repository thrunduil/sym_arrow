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

#include "sym_arrow/config.h"
#include "sym_arrow/fwd_decls.h"
#include "sym_arrow/nodes/type.h"

namespace sym_arrow { namespace error
{

namespace sd = sym_arrow::details;
namespace sa = sym_arrow::ast;

class sema_error
{
    public:
        void    null_expr();
        void    bind_array_not_set();
        void    invalid_bind_size(size_t size, size_t exp_size);

        void    const_symbol_substitution(const symbol& sym);
        void    invalid_substitution_no_convertion(const symbol& sym, const identifier& ty_ex);

        void    set_not_defined(const identifier& set);
        void    type_not_defined(const identifier& ty);

        void    set_already_defined(const identifier& set);
        void    type_already_defined(const identifier& set);
        void    symbol_already_defined(const identifier& set);
        void    set_elem_multiply_defined(const identifier& set_elem);

        void    undefined_symbol(const identifier& sym, size_t n_args);
        void    invalid_symbol_args(const identifier& sym, size_t n_args, 
                    const std::vector<identifier>& def_args, const type& def_type);
        void    invalid_symbol_arg(const identifier& sym, size_t arg, const identifier& t_arg, 
                    const std::vector<identifier>& def_args, const type& def_type);
        void    invalid_explicit_symbol_type(const identifier& sym, 
                    const std::vector<identifier>& def_args, const type&  def_type, 
                    const type& loc_type);

        void    invalid_symbol_nonconst_def(const identifier& sym, 
                    const std::vector<identifier>& def_args, const type&  def_type);

    private:
        void    disp_symbol_def(std::ostream& os, const identifier& sym, 
                    const std::vector<identifier>& def_args, const type& def_type);
};

}}
