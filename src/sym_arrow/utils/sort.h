/* 
 *  This file is a part of sym_arrow library.
 *
 *  Copyright (c) Pawe� Kowal 2017 - 2021
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

namespace sym_arrow { namespace utils
{

// insertion sort or array ptr of given size
template<class Value_type, class Comparer> 
void sort_ins(Value_type* ptr, size_t size, Comparer comp);

// quicksort of array ptr of given size; use sort_ins for
// short arrays
template<class Value_type, class Comparer> 
void sort_q(Value_type* ptr, size_t size, Comparer comp);

};};

#include "sym_arrow/utils/sort.inl"
