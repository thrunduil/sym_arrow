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

#include "string_data.inl"
#include "sym_arrow/ast/ast_tags.h"
#include "sym_arrow/ast/symbol_rep.h"
#include "sym_arrow/ast/ast.h"

namespace sym_arrow { namespace ast { namespace details
{

string_data::string_data(const char* ptr, size_t size, size_t hash)
{
    if (size == 0 || ptr == nullptr)
    {
        m_name = nullptr;
        return;
    }

    using context   = sym_dag::dag_context<term_tag>;

    char* tmp       = (char*)context::get().malloc((size + 1)* sizeof(char) + 2 * sizeof(size_t));

    memcpy_s(tmp, sizeof(size_t), reinterpret_cast<char*>(&size), sizeof(size_t));
    tmp             += sizeof(size_t);

    memcpy_s(tmp, sizeof(size_t), reinterpret_cast<char*>(&hash), sizeof(size_t));
    tmp             += sizeof(size_t);

    memcpy_s(tmp, size, ptr, size);
    tmp[size]       = 0;   

    m_name          = tmp;
};

void string_data::destroy()
{
    if (m_name == nullptr)
        return;

    char* ptr_root  = m_name - 2 * sizeof(size_t);

    using context   = sym_dag::dag_context<term_tag>;
    size_t N        = (this->get_size()  + 1) * sizeof(char) + 2 * sizeof(size_t);

    context::get().free(ptr_root, N);

    m_name          = nullptr;
};

};};}
