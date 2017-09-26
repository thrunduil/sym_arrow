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

#include "sym_arrow/ast/cannonization/item_collector.inl"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast
{

//----------------------------------------------------------------
//                  item_collector_add
//----------------------------------------------------------------
item_collector_add::item_collector_add(size_t size)
    : m_size(size), m_add(value::make_zero()), m_ih_array(size + 1 + buf_reserve)
    , m_capacity(size + buf_reserve)
{
    m_ih       = (item_handle*)m_ih_array.get();
    m_ih_tmp   = m_ih;
};

void item_collector_add::reserve(size_t n_new_elems)
{
    if (m_size + n_new_elems < m_capacity)
    {
        m_size += n_new_elems;
        return;
    }

    m_ih_array.reserve(n_new_elems);

    m_ih        = (item_handle*)m_ih_array.get();
    m_ih_tmp    = m_ih + m_size;

    m_size      += n_new_elems;
};

void item_collector_add::add_log(const value& scal, const expr& ex)
{
    expr tmp        = power_real(ex, scal);

    if (m_log.is_null())
        m_log   = std::move(tmp);
    else
        m_log   = std::move(m_log) * std::move(tmp);
};

void item_collector_add::add_log(const value& scal, expr&& ex)
{
    expr tmp        = power_real(std::move(ex), scal);

    if (m_log.is_null())
        m_log   = std::move(tmp);
    else
        m_log   = std::move(m_log) * std::move(tmp);
};

void item_collector_add::remove_empty()
{
    if (m_size == 0)
        return;

    size_t n            = m_size;
    item_handle* cur    = m_ih;
    item_handle* last   = m_ih + (n - 1);

    while (cur < last)
    {
        if (cur->m_expr == nullptr)
        {
            std::swap(*cur, *last);
            --last;
            --n;
        }
        else
        {
            ++cur;
        };
    };

    if (last->m_expr == nullptr)
        --n;

    set_size(n);
};

//----------------------------------------------------------------
//                  item_collector_mult_arrays
//----------------------------------------------------------------

item_collector_mult_arrays::item_collector_mult_arrays(const collector_size& size)
    : m_rih_array(size.rsize()), m_iih_array(size.isize()+1)
{};

//----------------------------------------------------------------
//                  item_collector_mult
//----------------------------------------------------------------
item_collector_mult::item_collector_mult(collector_size& size, 
                            item_collector_mult_arrays& arr)
    : m_scal(value::make_one()), m_size(size)
{
    m_rih       = (ritem_handle*)arr.get_ritem_array().get();
    m_iih       = (iitem_handle*)arr.get_iitem_array().get();
    m_iih_tmp   = m_iih;
    m_rih_tmp   = m_rih;

    if (size.rsize() > 0)
        ::memset(m_rih, 0, size.rsize() * sizeof(ritem_handle));        
};

item_collector_mult::item_collector_mult(collector_size& size, iitem_handle* ih, 
                    ritem_handle* rh)
    : m_scal(value::make_one()), m_size(size)
{
    m_rih       = rh;
    m_iih       = ih;
    m_iih_tmp   = m_iih;
    m_rih_tmp   = m_rih;
};

void item_collector_mult::collect(const mult_rep* am, int pow)
{
    {
        size_t n = am->isize();
        for (size_t i = 0; i < n; ++i)
        {
            expr_handle tmp_ex  = am->IE(i);
            int tmp_pow         = am->IV(i) * pow;

            add(tmp_pow, tmp_ex);
        };
    };
    {
        size_t n = am->rsize();
        for (size_t i = 0; i < n; ++i)
        {
            expr_handle tmp_ex      = am->RE(i);
            const value& eh2        = am->RV(i);
            value tmp_pow           = eh2 * value::make_value(pow);

            add(std::move(tmp_pow), tmp_ex);
        };
    };

    if (am->has_exp())
    {
        expr exp(am->Exp());
        add_exp(std::move(exp) * value::make_value(pow));
    };
};

void item_collector_mult::collect(const mult_rep* am, const value& pow)
{
    {
        size_t n = am->isize();
        for (size_t i = 0; i < n; ++i)
        {
            expr_handle tmp_ex  = am->IE(i);
            auto tmp_pow        = pow*value::make_value(am->IV(i));

            add(std::move(tmp_pow), tmp_ex);
        };
    };
    {
        size_t n = am->rsize();
        for (size_t i = 0; i < n; ++i)
        {
            expr_handle tmp_ex      = am->RE(i);
            const value& eh2        = am->RV(i);
            value tmp_pow           = eh2 * pow;

            add(tmp_pow, tmp_ex);
        };
    };

    if (am->has_exp())
    {
        expr exp(am->Exp());
        add_exp(pow * std::move(exp));
    };
};

void item_collector_mult::collect_base()
{
    if (this->rsize() == 0 || this->isize() == 0)
        return;

    // map based implementation is fast enough
    struct info
    {
        int     pos_r;
        int     pos_i;
        int     pow_i;

        info(int posr) :pos_r(posr), pos_i(-1), pow_i(0) {};
    };

    using base_map          = std::map<expr_handle, info>;
    using info_vec          = mmlib::details::stack_array<info*>;
    using int_vec           = mmlib::details::stack_array<int>;
    using base_value_type   = base_map::value_type;

    base_map    m_base_map;    

    for (size_t i = 0; i < rsize(); ++i)
    {
        if (m_rih[i].is_special() == true)
            continue;

        expr_handle eh      = m_rih[i].get_expr_handle();

        m_base_map.insert(base_value_type(eh, info((int)i)));
    };

    size_t ipos     = 0;
    size_t isize    = this->isize();

    info_vec m_process_vec(isize);
    int vec_pos = 0;

    while (ipos < isize)
    {
        if (m_iih[ipos].is_special() == true)
            continue;

        expr_handle eh      = m_iih[ipos].get_expr_handle();

        auto pos            = m_base_map.find(eh);

        if (pos == m_base_map.end())
        {
            ++ipos;
            continue;
        };

        info& in            = pos->second;
        if (in.pos_i == -1)
        {
            in.pos_i        = (int)ipos;
            m_process_vec.get()[vec_pos++] = &in;
        };

        int pow             = m_iih[ipos]->get_value();
        in.pow_i            += pow;

        ++ipos;
        continue;
    };

    if (vec_pos == 0)
        return;

    info** info_ptr     = m_process_vec.get();

    int_vec process_i(vec_pos);
    int_vec process_r(vec_pos);

    int* process_i_ptr  = process_i.get();
    int* process_r_ptr  = process_r.get();

    int vec_pos_r       = 0;
    int vec_pos_i       = 0;

    for (int i = 0; i < vec_pos; ++i)
    {
        int pos_r       = info_ptr[i]->pos_r;
        int pos_i       = info_ptr[i]->pos_i;
        int pow_i       = info_ptr[i]->pow_i;

        if (pow_i == 0 || pow_i == 1)
            continue;

        int pow_1;

        if (pow_i % 2 == 0)
        {
            pow_1       = pow_i;
        }
        else
        {
            pow_1       = pow_i - 1;
        };

        m_rih[pos_r].m_value = m_rih[pos_r].m_value + value::make_value(pow_1);

        if(m_rih[pos_r].m_value.is_zero() == true)
            process_r_ptr[vec_pos_r++]  = pos_r;

        int pow_sum             = m_iih[pos_i].m_value - pow_1;
        m_iih[pos_i].m_value    = pow_sum;

        if (pow_sum == 0)
            process_i_ptr[vec_pos_i++]  = pos_i;
    };

    if (vec_pos_r > 0)
    {
        std::sort(process_r_ptr, process_r_ptr + vec_pos_r);

        size_t rsize = this->rsize();

        for (int i = vec_pos_r - 1; i >= 0; --i)
        {
            int rpos    = process_r_ptr[i];
            m_rih[rpos] = std::move(m_rih[rsize - 1]);

            --rsize;
            --m_size.rsize();
        };
    };

    if (vec_pos_i > 0)
    {
        std::sort(process_i_ptr, process_i_ptr + vec_pos_i);

        size_t isize2 = this->isize();

        for (int i = vec_pos_i - 1; i >= 0; --i)
        {
            int ipos2    = process_i_ptr[i];
            m_iih[ipos2] = std::move(m_iih[isize2 - 1]);

            --isize2;
            --m_size.isize();
        };
    };
};

};};
    