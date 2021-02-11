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

#include "sym_arrow/fwd_decls.h"

namespace sym_arrow { namespace ast { namespace details
{

// represent value of given type as a POD type with the same sizeof
template<class Value_type>
struct pod_type
{    
    char m_data[sizeof(Value_type)];

    Value_type&         cast()          { return *reinterpret_cast<Value_type*>(&m_data); };
    const Value_type&   cast() const    { return *reinterpret_cast<const Value_type*>(&m_data); };
};

// calculate 2^val
SYM_ARROW_FORCE_INLINE
constexpr size_t pow2(size_t val)
{
    return size_t(1) << val;
};
}}}