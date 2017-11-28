/*
*  Morfa programming language.
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

#include "sym_arrow/ast/expr_cache.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//-------------------------------------------------------------------
//                      expr_cache
//-------------------------------------------------------------------
template<size_t Cache_size, size_t Num_rounds>
expr_cache<Cache_size, Num_rounds>::expr_cache()
    : m_curr_pos(0), m_last_pos(cache_size), m_all_filled(false)
{
    sym_dag::registered_dag_context::get().register_cache(this);
};

template<size_t Cache_size, size_t Num_rounds>
expr_cache<Cache_size, Num_rounds>::~expr_cache()
{
    clear();
}

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::rotate_buffer()
{
    static const size_t max_pos = cache_size * num_round;

    if (m_track_func)
        report_remove();

    if (m_curr_pos == max_pos)        
    {
        clear(0, cache_size);
        m_curr_pos      = 0;
        m_last_pos      = cache_size;
        m_all_filled    = true;
        return;
    };

    m_last_pos      = m_curr_pos + cache_size;

    if (m_all_filled == false)
        return;

    clear(m_curr_pos, m_last_pos);
    return;
};

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::clear(size_t first, size_t last)
{
    using dag_context   = expr_base::context_type;
    using stack_handle  = dag_context::stack_handle;
    using stack_type    = dag_context::stack_type;

    dag_context& c      = dag_context::get();
    stack_handle sh     = c.get_stack();
    stack_type& vec     = sh.get();

    for (size_t i = first; i < last; ++i)
    {
        expr_ptr* ex    = reinterpret_cast<expr_ptr*>(m_buf + i);
        vec.push_back(const_cast<expr_ptr&>(*ex).release());
    }
};

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::report_remove()
{
    size_t rem_first;
    size_t rem_last;
    size_t alive_first_1;
    size_t alive_first_2;
    size_t alive_last_1;
    size_t alive_last_2;

    static const size_t max_pos = cache_size * num_round;

    if (m_curr_pos == max_pos)        
    {
        alive_first_1   = 0;
        alive_last_1    = 0;

        rem_first       = 0;
        rem_last        = cache_size;
        
        alive_first_2   = cache_size;
        alive_last_2    = max_pos;
    }
    else if (m_all_filled == false)
    {
        return;
    }
    else
    {
        alive_first_1   = 0;
        alive_last_1    = m_curr_pos;
        
        rem_first       = m_curr_pos;
        rem_last        = m_curr_pos + cache_size;
        
        alive_first_2   = m_curr_pos + cache_size;
        alive_last_2    = max_pos;
    }

    for (size_t i = rem_first; i < rem_last; ++i)
    {
        expr_ptr& ex    = *reinterpret_cast<expr_ptr*>(m_buf + i);
        ex->set_user_flag<ast_flags::work>(false);
    }

    for (size_t i = alive_first_1; i < alive_last_1; ++i)
    {
        expr_ptr& ex    = *reinterpret_cast<expr_ptr*>(m_buf + i);
        ex->set_user_flag<ast_flags::work>(true);
    }

    for (size_t i = alive_first_2; i < alive_last_2; ++i)
    {
        expr_ptr& ex    = *reinterpret_cast<expr_ptr*>(m_buf + i);
        ex->set_user_flag<ast_flags::work>(true);
    }

    using dag_context   = expr_base::context_type;
    using stack_handle  = dag_context::stack_handle;
    using stack_type    = dag_context::stack_type;

    dag_context& c      = dag_context::get();
    stack_handle sh     = c.get_stack();
    stack_type& vec     = sh.get();

    for (size_t i = rem_first; i < rem_last; ++i)
    {
        expr_ptr& ex    = *reinterpret_cast<expr_ptr*>(m_buf + i);
        bool flag       = ex->get_user_flag<ast_flags::work>();

        if (flag == false)
            m_track_func(ex.get(), vec);
    }
};

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::set_track_function(const track_function& f)
{
    m_track_func = f;
};

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::close()
{
    m_curr_pos      = 0;
    m_last_pos      = cache_size;
    m_all_filled    = false;
};

template<size_t Cache_size, size_t Num_rounds>
void expr_cache<Cache_size, Num_rounds>::clear()
{
    if (m_all_filled == false)
    {
        clear(0, m_curr_pos);

        m_curr_pos      = 0;
        m_last_pos      = cache_size;
        m_all_filled    = false;
        
        return;
    }

    static const size_t max_pos = cache_size * num_round;

    // first first element in current block of cache_size
    // elements
    size_t first_curr   = (m_curr_pos / cache_size) * cache_size;    
    size_t last_curr    = first_curr + cache_size;

    if (last_curr == max_pos)
    {
        // positions [0, m_curr_pos) are filled;
        // positions [m_curr_pos, max_pos) are empty
        clear(0, m_curr_pos);
    }
    else
    {
        // positions [0, first_curr) are filled
        // positions [first_curr, m_curr_pos) are filled;
        // positions [m_curr_pos, last_curr) are empty;
        // positions [last_curr, max_pos) are filled

        clear(0, m_curr_pos);        
        clear(last_curr, max_pos);
    };

    m_curr_pos      = 0;
    m_last_pos      = cache_size;
    m_all_filled    = false;
};

};};
