/* 
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

#include "dag/details/vector_provider.h"

namespace sym_dag
{

//-------------------------------------------------------------------
//                  vector_handle
//-------------------------------------------------------------------
template<class Type>
inline vector_handle<Type>::vector_handle(vector_provider<Type>* owner, 
                                          vector_type* vec)
    :m_vector(vec), m_owner(owner)
{};

template<class Type>
inline vector_handle<Type>::vector_handle(vector_handle&& other)
    :m_vector(other.m_vector), m_owner(other.m_owner)
{
    other.m_vector = nullptr;
};

template<class Type>
inline vector_handle<Type>::~vector_handle()
{
    if (!m_vector)
        return;

    m_owner->release_vector(m_vector);
}

template<class Type>
inline typename vector_handle<Type>::vector_type& 
vector_handle<Type>::get()
{
    return *m_vector;
}

//-------------------------------------------------------------------
//                  vector_provider
//-------------------------------------------------------------------
template<class Type>
inline void vector_provider<Type>::release_vector(vector_type* vec)
{
    vec->clear();

    if (m_pool.size() < max_pool_size && vec->size() < max_vec_size)
        m_pool.push_back(vec);
    else
        delete vec;
};

template<class Type>
inline vector_handle<Type> vector_provider<Type>::get_vector()
{
    vector_type* vec  = get_vector_ptr();
    return vector_handle<Type>(this, vec);
};

template<class Type>
inline
typename vector_provider<Type>::vector_type* 
vector_provider<Type>::get_vector_ptr()
{
    if (m_pool.empty() == true)
        append_pool();

    vector_type* vec  = m_pool.back();
    m_pool.pop_back();
    return vec;
}

template<class Type>
inline vector_provider<Type>& vector_provider<Type>::get_global()
{
    return *m_global;
};

template<class Type>
vector_provider<Type>*
vector_provider<Type>::m_global = global_objects::make_after<vector_provider<Type>>();

template<class Type>
vector_provider<Type>::vector_provider()
{};

template<class Type>
vector_provider<Type>::~vector_provider()
{
    for (auto elem : m_pool)
        delete elem;
}

template<class Type>
void vector_provider<Type>::append_pool()
{
    m_pool.push_back(new vector_type());
    m_pool.back()->reserve(min_vec_size);
};

};
