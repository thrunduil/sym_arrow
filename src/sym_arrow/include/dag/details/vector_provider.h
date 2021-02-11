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

#include <vector>
#include "dag/details/global_objects.h"

namespace sym_dag
{

template<class Type>
class vector_handle;

// a singleton class that provides work vectors for
// cannonize class; this class is intended to reuse work
// vectors instead of costly allocation and deallocation vectors
// on demand
template<class Type>
class vector_provider
{
    private:
        static const size_t min_vec_size    = 10;
        static const size_t max_vec_size    = 1000;
        static const size_t max_pool_size   = 20;

    public:
        using vector_type           = std::vector<Type>;
        using handle_type           = vector_handle<Type>;

    private:
        using vector_pool           = std::vector<vector_type*>;

    private:
        static vector_provider*     m_global;
        vector_pool                 m_pool;

    private:        
        friend vector_handle<Type>;

        vector_provider();

        vector_provider(const vector_provider&) = delete;
        vector_provider& operator=(const vector_provider&) = delete;

        void                        append_pool();

        friend global_objects;

    public:
        ~vector_provider();

        static vector_provider&     get_global();

        // return a handle to vector; this handle is responsible for releasing
        // memory; release_vector function cannot be called on a work vector stored
        // in the handle
        vector_handle<Type>         get_vector();

        // return pointer to a vector; this vector must be released
        // later by calling release_vector function
        vector_type*                get_vector_ptr();

        // release memory
        void                        release_vector(vector_type* vec);
};

// a RAII class that wraps a work vector; at destructor stored
// working vector is cleared and returned to vector_provider
template<class Type>
class vector_handle
{
    public:
        using vector_type           = std::vector<Type>;
        using provider_type         = vector_provider<Type>;

    private:
        vector_provider<Type>*      m_owner;
        vector_type*                m_vector;

    public:
        // move constructor
        vector_handle(vector_handle&& other);

        // stored work vector is cleared and returned to vector_provider 
        // for later use
        ~vector_handle();

        // return working vector
        vector_type&                get();

    private:
        vector_handle(vector_provider<Type>* owner, vector_type* vec);

        vector_handle(const vector_handle&) = delete;
        vector_handle& operator=(const vector_handle&) = delete;

        friend vector_provider<Type>;
};

};

#include "dag/details/vector_provider.inl"