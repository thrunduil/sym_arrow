/*
*  Morfa programming language.
*  This file is a part of sym_arrow library.
*
*  Copyright (c) Pawe³ Kowal 2017-2019
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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/functions/contexts.h"

namespace sym_arrow
{

//----------------------------------------------------------------------
//                        set functions
//----------------------------------------------------------------------

// return true if sets defined by symbol A contains the element x and false
// otherwise; exception is thrown if A does not define a set
bool SYM_ARROW_EXPORT   set_is_member(const identifier& A, const expr& x);

// return true if sets s1 and s2 are equal, i.e. contains the same elements
// sets are equal iff their addresses are equal
bool SYM_ARROW_EXPORT    operator==(const set& s1, const set& s2);

// return true if sets s1 and s2 are different, i.e. contains the different
// elements
bool SYM_ARROW_EXPORT    operator!=(const set& s1, const set& s2);

// comparison function based on addresses of pointers;
bool SYM_ARROW_EXPORT    operator<(const set& s1, const set& s2);
bool SYM_ARROW_EXPORT    operator>(const set& s1, const set& s2);
bool SYM_ARROW_EXPORT    operator<=(const set& s1, const set& s2);
bool SYM_ARROW_EXPORT    operator>=(const set& s1, const set& s2);

};

