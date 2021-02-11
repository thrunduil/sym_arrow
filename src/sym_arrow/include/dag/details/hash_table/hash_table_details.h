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

#include <new>
#include <boost/functional/hash.hpp>


namespace sym_dag { namespace details
{

struct ht_base
{
    size_t				n_elements;
    mutable size_t	    n_removed;

    virtual void        check_shrink() = 0;
};

class ht_capacity_prime
{
    public:
        static const size_t min_size = 3;

    private:
        size_t              m_value;
        size_t              m_pos;

    public:
        ht_capacity_prime(size_t size);

        size_t              value() const;
        ht_capacity_prime   next_size() const;
        ht_capacity_prime   previous_size() const;
        size_t              project(size_t val) const;

    private:
        static size_t       get_value(size_t pos);
};
class ht_capacity_pow2
{
    public:
        static const size_t min_size = 4;

    private:
        size_t              m_value;
        size_t              m_mask;

    public:
        ht_capacity_pow2(size_t size);

        size_t              value() const;
        ht_capacity_pow2    next_size() const;
        ht_capacity_pow2    previous_size() const;
        size_t              project(size_t val) const;

    private:
        ht_capacity_pow2(size_t val, int dum);
};

template<class T,class HT>  struct assign_enabler                 { using type = void*; };
template<class T,class HT>  struct assign_enabler<T, const HT>    { };

struct check_assign_true{};
struct check_assign_false{};

};};