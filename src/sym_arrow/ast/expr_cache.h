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

#include "sym_arrow/nodes/expr.h"
#include "sym_arrow/ast/helpers/utils.h"

namespace sym_arrow { namespace ast
{

enum class track_function_code : size_t
{
    cse,
    diff
};

// buffer of expression, that can store Cache_size x Num_rounds
// elements; when the buffer is full, then oldest k elements are
// removed (k = Cache_size)
template<size_t Cache_size, size_t Num_rounds>
class expr_cache : public sym_dag::node_cache
{
    private:
        static const size_t cache_size  = Cache_size;
        static const size_t num_round   = Num_rounds;

    public:
        using stack_type    = expr_base::stack_type;
        using track_function= std::function<void (expr_handle, stack_type&)>;        

    private:
        using expr_pod      = details::pod_type<expr_ptr>;        

    private:
        expr_pod            m_buf[cache_size * num_round];

        size_t              m_curr_pos;
        size_t              m_last_pos;
        bool                m_all_filled;
        track_function      m_track_func;

        expr_cache(const expr_cache&) = delete;
        expr_cache& operator=(const expr_cache&) = delete;

    public:
        expr_cache();
        ~expr_cache();

        void                add(const expr_ptr& ex);
        virtual void        clear() override;

        //clear cache; destructors are not called
        void                close();

        // define function, that will be called on every element
        // that is removed from the buffer
        void                set_track_function(const track_function& f);

    private:
        void                rotate_buffer();
        void                clear(size_t first, size_t last);
        void                report_remove();
};

template<size_t Cache_size, size_t Num_rounds>
inline void expr_cache<Cache_size, Num_rounds>::add(const expr_ptr& ex)
{
    new (m_buf + m_curr_pos) expr_ptr(ex);
    ++m_curr_pos;

    if (m_curr_pos == m_last_pos)
        rotate_buffer();
};

};};

#include "sym_arrow/ast/expr_cache.inl"
