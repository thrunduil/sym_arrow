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

namespace sym_arrow { namespace error
{

namespace sd = sym_arrow::details;
namespace sa = sym_arrow::ast;

class sema_error
{
    public:
        void    set_not_defined(const identifier& set);
        void    type_not_defined(const identifier& ty);

        void    set_already_defined(const identifier& set);
        void    type_already_defined(const identifier& set);
        void    symbol_already_defined(const identifier& set);
        void    set_elem_multiply_defined(const identifier& set_elem);

        void    unable_subs_index_set_different(const index& i1, const index& i2);
        void    unable_subs_index_not_member(const index& i1, const expr& member);
};

}}
