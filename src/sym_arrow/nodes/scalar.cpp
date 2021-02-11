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

#include "sym_arrow/details/scalar.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow
{

scalar::scalar(double v)
    :m_ptr(value_type::make(v))
{};

scalar::scalar(const value& v)
    :m_ptr(value_type::make(v))
{};

scalar::~scalar()
{};

const value& scalar::get_value() const
{
    return m_ptr->static_cast_to<ast::scalar_rep>()->get_data();
};

const scalar& scalar::make_zero()
{
    return ast::scalar_rep::make_zero();
};

const scalar& scalar::make_one()
{
    return ast::scalar_rep::make_one();
};

const scalar& scalar::make_minus_one()
{
    return ast::scalar_rep::make_minus_one();
};

};