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

#include "sym_arrow/ast/scalar_rep.inl"
#include "sym_arrow/ast/ast.h"
#include "term_context_data.h"

namespace sym_arrow { namespace ast
{

const scalar& scalar_rep::make_zero()
{
    return context_type::get().get_context_data().get_scalar_zero();
};

const scalar& scalar_rep::make_nan()
{
    return context_type::get().get_context_data().get_scalar_nan();
};

const scalar& scalar_rep::make_one()
{
    return context_type::get().get_context_data().get_scalar_one();
};

const scalar& scalar_rep::make_minus_one()
{
    return context_type::get().get_context_data().get_scalar_minus_one();
};

};};
