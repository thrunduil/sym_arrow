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

#pragma once

#include "sym_arrow/nodes/value.h"
#include <limits>

namespace sym_arrow
{

//---------------------------------------------------------------------
//                  value
//---------------------------------------------------------------------
inline size_t value::hash_value() const
{ 
    return eval_hash(*this); 
};

inline bool value::equal(const value& val) const
{ 
    return operator==(*this,val); 
};

inline bool value::equal(double val) const
{ 
    return operator==(*this, make_value(val)); 
};

inline value& value::operator*=(const value& v)
{ 
    return *this = operator*(*this, v); 
};

inline value& value::operator+=(const value& v)
{ 
    return *this = operator+(*this, v); 
};

inline double value::get_value_abs() const
{
    return ::abs(this->get_value());
}

inline bool value::is_zero() const
{ 
    return m_data == 0.0;
}

inline bool value::is_one() const
{ 
    return m_data == 1.0;
}

inline bool value::is_minus_one() const
{ 
    return m_data == -1.0;
}

inline bool value::is_positive() const
{ 
    return m_data > 0.0;
}

inline bool value::is_negative() const
{ 
    return m_data < 0.0;
}

};