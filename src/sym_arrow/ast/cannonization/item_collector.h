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

#include "sym_arrow/ast/builder/build_item.h"
#include "sym_arrow/ast/helpers/utils.h"
#include "sym_arrow/nodes/expr.h"
#include "mmlib_internals/utils/stack_array.h"

namespace sym_arrow { namespace ast
{

class item_collector_add
{
    public:
        using item          = build_item<value>;
        using item_handle   = item::handle_type;

    private:
        // buf_reserve is used when after common subexpr elimination
        // new elements must be added
        static const size_t buf_reserve = 5;

        static const size_t stack_size  = 10 + buf_reserve;        

        using stack_elem    = mmlib::details::pod_type<item_handle>;
        using item_array    = mmlib::details::stack_array<stack_elem, stack_size>;

    private:
        size_t              m_size;
        size_t              m_capacity;
        expr                m_log;
        value               m_add;

        item_array          m_ih_array;
        item_handle*        m_ih;
        item_handle*        m_ih_tmp;

    public:
        explicit item_collector_add(size_t size);

        void                test() const;

        size_t              size() const;

        bool                has_log() const;
        item_handle*        get_ih();
        const value&        get_add() const;
        const expr&         get_log() const;        

        void                set_size(size_t n);
        void                remove_empty();
        void                reserve(size_t n_new_elems);

    public:
        void                add_add(const value& add);
        void                add_log(const value& scal, const expr& ex);
        void                add_log(const value& scal, expr&& ex);
        void                add(const value& scal, expr_handle tmp_ex);

        void                set_log(const expr& ex);
        void                set_add(const value& add);
};

class item_collector_size
{
    private:
        size_t          m_in;
        size_t          m_rn;

    public:
        item_collector_size();
        item_collector_size(size_t isize, size_t rsize);

        size_t          isize() const       { return m_in;};
        size_t          rsize() const       { return m_rn;};

    public:        
        void            report_special();

        template<bool Is_ilist>
        void            report_mult(const mult_rep* am);

        template<bool Is_ilist>
        void            report_atom();

        template<bool is_ilist>
        void            add(const item_collector_size& other);

    private:
        size_t&         isize()             { return m_in;};
        size_t&         rsize()             { return m_rn;};

        friend class item_collector_mult;
};

class item_collector_mult_arrays
{
    private:
        using iitem_handle      = build_item_handle<int>;
        using ritem_handle      = build_item_handle<value>;
        using collector_size    = item_collector_size;

        using ritem_array       = mmlib::details::stack_array<mmlib::details::pod_type<ritem_handle>>;
        using iih_array         = mmlib::details::stack_array<mmlib::details::pod_type<iitem_handle>>;

    private:
        ritem_array         m_rih_array;        
        iih_array           m_iih_array;        

    public:
        explicit item_collector_mult_arrays(const collector_size& size);

        ritem_array&        get_ritem_array()   { return m_rih_array; }
        iih_array&          get_iitem_array()   { return m_iih_array; };
};

class item_collector_mult
{
    private:
        using iitem_handle      = build_item_handle<int>;
        using ritem_handle      = build_item_handle<value>;
        using collector_size    = item_collector_size;

        using ritem_array       = mmlib::details::stack_array<mmlib::details::pod_type<ritem_handle>>;
        using iih_array         = mmlib::details::stack_array<mmlib::details::pod_type<iitem_handle>>;
        using value_type        = typename ritem_array::value_type;
        using arrays            = item_collector_mult_arrays;

    private:
        ritem_handle*       m_rih_tmp;
        iitem_handle*       m_iih_tmp;

        collector_size&     m_size;
        value               m_scal;
        expr                m_exp;

        ritem_handle*       m_rih;
        iitem_handle*       m_iih;

    public:
        item_collector_mult(collector_size& size, item_collector_mult_arrays& arr);

        item_collector_mult(collector_size& size, iitem_handle* ih, 
                            ritem_handle* rh);

        void                test() const;

        size_t              isize() const                   { return m_size.isize();};
        size_t              rsize() const                   { return m_size.rsize();};                

        bool                has_exp() const;

        iitem_handle*       get_iih()                       { return m_iih; };
        ritem_handle*       get_rih()                       { return m_rih; };
        value               get_scal()                      { return m_scal; };
        const expr&         get_exp()                       { return m_exp; };


        void                add_scal(const value& s)        { m_scal = m_scal * s; };
        void                add_exp(const expr& v);
        void                add_exp(expr&& v);

        void                add(int pow, expr_handle tmp_ex);
        void                add(const value& pow, expr_handle tmp_ex);

        void                collect(const mult_rep* am, int p);
        void                collect(const mult_rep* am, const value& p);
        void                collect_base();

        void                make_power(const value& v, int p);
        void                make_power(const value& v, const value& p);
};

};};
    