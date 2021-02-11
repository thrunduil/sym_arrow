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

#include "dag/config.h"
#include "dag/dag_ptr.h"

#include <vector>

namespace sym_dag { namespace details
{

// calculate number of bits required to store codes up to code
template<size_t code>
struct calc_number_bits
{
    static const size_t value   = 1 + calc_number_bits<code/2>::value;
};

template<>
struct calc_number_bits<0>
{
    static const size_t value   = 0;
};

// calculate number of bits required to store node codes
template<class Tag>
struct calculate_code_bits
{
    static const size_t num_codes   = dag_tag_traits<Tag>::number_codes;

    //number of bits must be at least 1
    static const size_t last_code   = (num_codes > 1) ? (num_codes - 1) : 1;

    static const size_t value       = calc_number_bits<last_code>::value;
};

// get number of user bits in dag_item_base
template<class Tag>
struct calculate_flag_bits
{
    static const size_t max         = 3;
    static const size_t value       = dag_tag_traits<Tag>::user_flag_bits;

    static_assert(value <= max, "too many bits of uset flags in dag_item");
};

// header of dag_item_base (reference counter, user flags, user data and 
// node codes); it is assumed that compiler is able to perform zero size
// base class optimization if Data is empty
template<class Tag, class Data>
struct dag_item_header : public Data
{
    static const size_t num_codes       = dag_tag_traits<Tag>::number_codes;

    static_assert(num_codes <= SYM_DAG_MAX_NUMBER_CODES, "too many codes");

    static const size_t code_bits       = calculate_code_bits<Tag>::value;
    static const size_t flag_bits       = calculate_flag_bits<Tag>::value;
    static const size_t reserved_bits   = 3;
    static const size_t total_bits      = reserved_bits + code_bits + flag_bits;
    static const size_t header_bits     = sizeof(size_t) * 8;
    static const size_t ref_bits        = header_bits - total_bits;
    static const size_t min_ref_bits    = 23;

    static_assert(total_bits < header_bits && ref_bits >= min_ref_bits, "not enough bits for refcount");

    static const size_t temporary_flag  = 0;
    static const size_t track_flag      = 1;
    static const size_t weak_flag       = 2;

    size_t      m_ref         : ref_bits;
    size_t      m_flags       : reserved_bits + flag_bits;    
    size_t      m_code        : code_bits;
};

// class responsible for reference counting updates
template<class Tag>
class dag_item_traits;

}};
