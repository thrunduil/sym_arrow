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

#include "dag/refptr.h"

namespace sym_dag
{

//-----------------------------------------------------------------
//                      refptr
//-----------------------------------------------------------------

template<class T, class TT>
SYM_DAG_FORCE_INLINE 
refptr<T,TT>::refptr()
    :m_value(nullptr)
{};

template<class T, class TT>
SYM_DAG_FORCE_INLINE 
refptr<T,TT>::refptr(T* p)
    :m_value(p)
{};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
refptr<T,TT>::refptr(T* p, copy_t)
    :m_value(p)
{
    TT::copy(m_value);
};

template<class T, class TT>
inline
refptr<T,TT> refptr<T,TT>::make(T* p)
{
    return refptr<T,TT>(p);
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
refptr<T,TT>::~refptr()
{
    TT::check_free(m_value);
};

template<class T, class TT>
template<class Y>
SYM_DAG_FORCE_INLINE 
refptr<T,TT>::refptr(refptr<Y,TT> const& r)    
:m_value(r.m_value)
{
    TT::copy(m_value);
};

template<class T, class TT>
template<class Y>
SYM_DAG_FORCE_INLINE 
refptr<T,TT>::refptr(refptr<Y,TT> && r)    
:m_value(std::move(r.m_value))
{
    r.m_value = nullptr;
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE 
refptr<T,TT>::refptr(refptr const& r)    
:m_value(r.m_value)
{
    TT::copy(m_value);
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
refptr<T,TT>::refptr(refptr && r)    
:m_value(std::move(r.m_value))
{
    r.m_value = nullptr;
};

template<class T, class TT>
template<class Y>
SYM_DAG_FORCE_INLINE
refptr<T,TT>& refptr<T,TT>::operator=(refptr<Y,TT> const & r)
{
    TT::assign(m_value,r.m_value);
    m_value = r.m_value;
    return *this;
};

template<class T, class TT>
template<class Y>
SYM_DAG_FORCE_INLINE
refptr<T,TT>& refptr<T,TT>::operator=(refptr<Y,TT> && r)
{
    refptr tmp(std::move(r));
    std::swap(m_value,tmp.m_value);

    return *this;
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
refptr<T,TT>& refptr<T,TT>::operator=(refptr const & r)
{
    TT::assign(m_value,r.m_value);
    m_value = r.m_value;
    return *this;
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
refptr<T,TT>& refptr<T,TT>::operator=(refptr && r)
{
    std::swap(m_value,r.m_value);
    return *this;
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
T* refptr<T,TT>::release()
{ 
    T* tmp  = m_value; 
    m_value = nullptr; 
    return tmp; 
};

template<class T, class TT>
template<class Stack_type>
SYM_DAG_FORCE_INLINE
void refptr<T,TT>::release(Stack_type& st)
{
    st.push_back(this->release());
}

template<class T, class TT>
template<class Y> 
inline
refptr<T,TT> refptr<T,TT>::from_this(Y* ptr)
{
    refptr out(ptr);
    TT::copy(out.m_value);
    return out;
};

template<class T, class TT>
SYM_DAG_FORCE_INLINE
void refptr<T,TT>::swap(refptr& other)
{
    T temp(std::move(m_value));
    m_value            = std::move(other.m_value);
    other.m_value    = std::move(temp);
};

};
