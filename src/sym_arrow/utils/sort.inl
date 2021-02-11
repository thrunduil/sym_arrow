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

#include "sym_arrow/utils/sort.h"

namespace sym_arrow { namespace utils
{

static const int SORT_MAX  = 50;

template <class Type, class Comparer>
inline void utils::sort_ins(Type * const ptr, const size_t n, Comparer comp)
{
    for (size_t j = 1; j < n; ++j)
    {
        Type x(std::move(ptr[j]));

        size_t i = j;

        for (; i >= 1 && comp(x, ptr[i - 1]); --i)
            ptr[i] = std::move(ptr[i - 1]);

        ptr[i] = std::move(x);
    };
};

template <typename Type, class Comparer> 
inline void sort_q(Type *pF, Type *pL, Comparer comp)
{
    auto n = pL-pF;

    if (n <= 1) 
        return;

    if (n <= SORT_MAX) 
    {
        sort_ins(pF, n, comp);
    } 
    else 
    {
        std::sort(pF, pL, comp);
    };
};


template <class Type, class Comparer>
inline void sort_q(Type * const ptr, const size_t n, Comparer comp)
{
    return sort_q(ptr, ptr+n, comp);
}

};};