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

#include "sym_arrow/nodes/expr.h"

namespace sym_arrow
{

inline const expr::ptr_type& expr::get_ptr() const
{
    return m_ptr;
};

inline bool expr::is_null() const
{
    return !m_ptr;
}

inline const expr* expr::operator->() const
{ 
    return this; 
};

inline ast::expr_handle expr::get_expr_handle() const
{ 
    return m_ptr.get(); 
};

};
