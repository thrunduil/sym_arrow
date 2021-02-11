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
#include "dag/details/allocator.h"
#include <boost/pool/pool.hpp>

namespace sym_dag { namespace details
{

#define DAG_MALLOC_MAX_SIZE 20

struct mem_manager_tag{};

class SYM_DAG_EXPORT mem_manager
{
    private:
        using allocator_type    = symbolic_allocator<mem_manager_tag>;
        using pool              = boost::pool<allocator_type>;

    private:
        mem_manager(const mem_manager&) = delete;
        mem_manager& operator=(mem_manager&) = delete;

    private:
        pool*       m_pools;

    public:
        mem_manager();
        ~mem_manager();        

        template<size_t words>
        void*       malloc();

        void*       malloc(size_t words);

        template<size_t words>
        void        free(void* ptr);

        void        free(void* ptr, size_t words);

        // all memory is released; free function cannot be called
        // for previously allocated objects, however malloc functions
        // can be called
        void        purge_memory();
};

template<size_t words>
inline void* mem_manager::malloc()
{ 
    return m_pools[words].malloc(); 
}

inline void* mem_manager::malloc(size_t words)
{ 
    return m_pools[words].malloc(); 
}

template<size_t words>
inline void mem_manager::free(void* ptr)
{     
    return m_pools[words].free(ptr); 
};

inline void mem_manager::free(void* ptr, size_t words)
{ 
    return m_pools[words].free(ptr); 
};

};};