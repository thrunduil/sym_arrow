/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe� Kowal 2017
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

#include "term_context_data.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast { namespace details
{

term_context_data::~term_context_data()
{};

void term_context_data::initialize()
{
    m_scalar_zero       = scalar(value::make_zero());
    m_scalar_one        = scalar(value::make_one());
    m_scalar_minus_one  = scalar(value::make_minus_one());
    m_scalar_nan        = scalar(value::make_nan());
};

void term_context_data::close()
{
    m_scalar_zero       = scalar();
    m_scalar_one        = scalar();
    m_scalar_minus_one  = scalar();

    m_reg_symbols.close(); 
};

}}};
