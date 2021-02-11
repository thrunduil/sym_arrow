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

#include "dag/details/hash_table/hash_equal.h"

namespace sym_dag { namespace details
{

template<class elem_type>
template<class info>
inline size_t obj_hasher<elem_type>::operator()(const info& pi) const
{
    return elem_type::eval_hash(pi);
};

template<class elem_type>
inline size_t obj_hasher<elem_type>::operator()(const elem_type* ptr1) const
{
    return ptr1->hash_value();
};

template<class elem_type>
template<class info>
inline bool obj_equaler<elem_type>::operator()(const elem_type* ptr1, 
                              size_t hash_value,const info& pi) const
{
    return ptr1->hash_value() == hash_value && ptr1->equal(pi);
};

template<class elem_type>
inline bool obj_equaler<elem_type>::operator()(const elem_type* ptr1,
                                  size_t, const elem_type* ptr2) const
{
    return ptr1 == ptr2;
};

};};
