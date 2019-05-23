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

#include "symbol_context_data.h"

namespace sym_arrow { namespace ast { namespace details
{

inline size_t symbol_context_data::get_fresh_ident_code()
{ 
    return m_reg_symbols->get_fresh_identifier_code();
};

inline void symbol_context_data::register_ident(const identifier_rep* h)
{ 
    m_reg_symbols->register_ident(h); 
};

inline void symbol_context_data::unregister_ident(const identifier_rep* h)
{
    m_reg_symbols->unregister_ident(h); 
};

inline const identifier& symbol_context_data::default_id() const
{
    return m_default_id;
}

}}};
