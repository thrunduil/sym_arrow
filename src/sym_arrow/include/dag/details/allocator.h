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

#include <memory>

namespace sym_dag { namespace details
{
    struct memory_stats;

    // throw exception
    SYM_DAG_EXPORT void report_bad_alloc();

    // reporting malloc and free; check memory leaks
    class SYM_DAG_EXPORT memory_debugger
    {
        public:
            // malloc is called
            template<class Tag>
            static void report_malloc(size_t bytes)
            {
                const char* name = typeid(Tag).name();
                report_malloc(name, bytes);
            };

            // free is called
            template<class Tag>
            static void report_free(size_t bytes)
            {
                const char* name = typeid(Tag).name();
                report_free(name, bytes);
            }

            static void disp_memory_stats(std::ostream& os, 
                            details::memory_stats& stats);

        private:
            static void report_malloc(const char* type_name, size_t size);
            static void report_free(const char* type_name, size_t size);
    };

#if SYM_DAG_DEBUG_MEMORY
    // memory allocator; Tag is used in debugging purpose only
    template<class Tag>
    struct symbolic_allocator
    {
        using size_type         = std::size_t;
        using difference_type   = std::ptrdiff_t;

        // allocate num_bytes bytes; throws exception if allocation fails
        static char* malloc(const size_type bytes)
        { 
            size_t* ptr = reinterpret_cast<size_t*>(::malloc(bytes + sizeof(size_t)));
            
            if (!ptr)
                report_bad_alloc();            

            *ptr = bytes;
            ++ptr;
        
            memory_debugger::report_malloc<Tag>(bytes);

            return reinterpret_cast<char*>(ptr);
        }
        
        // free previously allocated memory; block != nullptr
        static void free(void* block)
        { 
            size_t* ptr     = reinterpret_cast<size_t*>(block);
            --ptr;        

            size_t bytes    = ptr[0];
            memory_debugger::report_free<Tag>(bytes);
        
            ::free(ptr);
        }
    };

#else
    // memory allocator; Tag is used in debugging purpose only
    template<class Tag>
    struct symbolic_allocator
    {
        using size_type         = std::size_t;
        using difference_type   = std::ptrdiff_t;

        // allocate num_bytes bytes; throws exception if allocation
        // fails
        static char* malloc(const size_type num_bytes)
        { 
            size_t* ptr = reinterpret_cast<size_t*>(::malloc(num_bytes));
            if (!ptr)
                report_bad_alloc();            

            return reinterpret_cast<char*>(ptr);
        }

        // free previously allocated memory; block != nullptr
        static void free(void* block)
        { 
            size_t* ptr     = reinterpret_cast<size_t*>(block);        
            ::free(ptr);
        }
    };

#endif 

};};