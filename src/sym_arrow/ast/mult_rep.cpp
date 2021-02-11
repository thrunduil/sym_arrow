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

#include "sym_arrow/ast/mult_rep.inl"
#include "sym_arrow/ast/ast.h"
#include "sym_arrow/utils/stack_array.h"

namespace sym_arrow { namespace ast
{
 
namespace sd = sym_arrow :: details;

mult_rep::~mult_rep()
{
    using context_type  = sym_dag::dag_context<term_tag>;
    context_type& c     = context_type::get();

    for(size_t i = 0; i < m_int_size; ++i)
        m_int_data[i].~int_expr();

    for(size_t i = 0; i < m_real_size; ++i)
        m_real_data[i].~value_expr();

    if (has_exp())
        m_real_data[m_real_size].~value_expr();

    c.free(m_int_data, m_int_size * sizeof(int_expr) 
           + (m_real_size + (has_exp()? 1 : 0)) * sizeof(value_expr));
};

void mult_rep::release(stack_type& st)
{
    for(size_t i = 0; i < m_int_size; ++i)
        st.push_back(m_int_data[i].m_expr.release());

    for(size_t i = 0; i < m_real_size; ++i)
        st.push_back(m_real_data[i].m_expr.release());

    if (has_exp())
        st.push_back(m_real_data[m_real_size].m_expr.release());
};

void mult_rep::remove_exp(const mult_rep* h, expr& res)
{
    if (h->has_exp() == false)
    {
        res = expr(h);
        return;
    };

    if (h->isize() == 0 && h->rsize() == 0)
    {
        res = scalar_rep::make_one();
        return;
    };

    size_t in           = h->isize();
    size_t rn           = h->rsize();

    if (in == 1 && rn == 0 && h->IV(0) == 1)
    {
        res = expr(h->IE(0));
        return;
    };

    mult_rep_info<int_expr, value_expr> 
    info(in, h->m_int_data, nullptr, rn, h->m_real_data);

    expr_ptr res_ptr    = mult_rep::make(info);
    res                 = expr(res_ptr);
    return;
};

};};
