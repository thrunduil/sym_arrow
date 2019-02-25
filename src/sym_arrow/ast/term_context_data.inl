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

#include "term_context_data.h"

namespace sym_arrow { namespace ast { namespace details
{

inline const scalar& term_context_data::get_scalar_zero()
{
    return m_scalar_zero;
}

inline const scalar& term_context_data::get_scalar_nan()
{
    return m_scalar_nan;
}

inline const scalar& term_context_data::get_scalar_one()
{
    return m_scalar_one;
}

inline const scalar& term_context_data::get_scalar_minus_one()
{
    return m_scalar_minus_one;
}

inline size_t term_context_data::get_fresh_indexed_symbol_code()
{ 
    return m_free_codes.get_fresh_symbol_code(); 
};

inline void term_context_data::register_symbol(const indexed_symbol_rep* h)
{
    (void)h;
};

inline void term_context_data::unregister_symbol(const indexed_symbol_rep* h)
{
    size_t code     = h->get_indexed_symbol_code();
    m_free_codes.unregister_sym(code);
};

}}};
