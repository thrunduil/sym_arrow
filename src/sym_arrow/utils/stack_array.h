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

namespace sym_arrow { namespace details
{

template<class value_type>
struct pod_type;

template<class value_type>
struct stack_array_elem_destructor
{    
    virtual ~stack_array_elem_destructor()  {};
    virtual void    destroy(value_type*) = 0;    
};

template<class value_type>
struct pod_destructor : stack_array_elem_destructor<pod_type<value_type>>
{
    using elem_type = pod_type<value_type>;

    int*            m_size;

    pod_destructor(int* size)   
        : m_size(size)
    {};

    virtual void destroy(elem_type* ptr) override;
};

struct default_allocator_stack_array
{
    static void*    malloc(size_t size);
    static void     free(void* ptr, size_t size);
    static void*    realloc(void* ptr, size_t old_size, size_t new_size);
};

template<class value_type_, int n_elem = 10, class Allocator = default_allocator_stack_array>
class stack_array
{
    static_assert(std::is_pod<value_type_>::value,"value_type must be pod");

    public:        
        using value_type        = value_type_;
        using destructor_type   = stack_array_elem_destructor<value_type>;
        using allocator_type    = Allocator;

    private:
        struct header
        {
            size_t          m_size;

            header(size_t n)    
                :m_size(n) 
            {};
        };

    private:
        value_type			m_array[n_elem];
        value_type*			m_ptr;
        destructor_type*    m_destructor;

    private:
        stack_array(const stack_array&) = delete;
        stack_array& operator=(const stack_array&) = delete;

        void                bad_alloc() const;

    public:
        stack_array(size_t size);
        stack_array(size_t size, destructor_type* d);
        ~stack_array();

        // get pointer to first element in the array
        value_type*         get()       { return m_ptr? m_ptr : m_array; };        

        // get pointer to first element in the array; cast to
        // type T is performed
        template<class T>
        T*                  get_cast()  { return reinterpret_cast<T*>(get()); };

        // change array capacity to 'size'; all pointers
        // can be invalidated
        void                reserve(size_t size);
};

template<class value_type>
struct pod_type
{    
    using destructor_type   = pod_destructor<value_type>;

    char m_data[sizeof(value_type)];

    value_type&             cast();
    const value_type&       cast() const;
};

//-------------------------------------------------------------------
//                      IMPLEMENTATION
//-------------------------------------------------------------------

template<class value_type>
inline void pod_destructor<value_type>::destroy(elem_type* ptr)
{
    int n = *m_size;

    for (int i = 0; i < n; ++i)
        ptr[i].cast().~value_type();
};

template<class value_type>
inline value_type& pod_type<value_type>::cast()
{ 
    return *reinterpret_cast<value_type*>(&m_data); 
};

template<class value_type>
inline const value_type& pod_type<value_type>::cast() const
{ 
    return *reinterpret_cast<const value_type*>(&m_data); 
};

inline void* default_allocator_stack_array::malloc(size_t size)
{ 
    return ::malloc(size); 
};

inline void default_allocator_stack_array::free(void* ptr, size_t size)
{ 
    (void)size; 
    ::free(ptr); 
};

inline void* default_allocator_stack_array::realloc(void* ptr, size_t old_size, 
         size_t new_size)
{ 
    (void)old_size; 
    return ::realloc(ptr, new_size); 
};

template<class value_type, int n_elem, class Allocator>
stack_array<value_type,n_elem,Allocator>::stack_array(size_t size)	
    :m_ptr(nullptr), m_destructor(nullptr)
{
    if (size > n_elem)
    {
        header* tmp = (header*)Allocator::malloc(size*sizeof(value_type) + sizeof(header));

        if (!tmp)
            bad_alloc();

        tmp[0] = header(size);
        ++tmp;
        m_ptr = (value_type*)tmp;
    };
};

template<class value_type, int n_elem, class Allocator>
stack_array<value_type,n_elem,Allocator>::stack_array(size_t size, destructor_type* d)	
    :m_ptr(nullptr), m_destructor(d)
{
    if (size > n_elem)
    {
        header* tmp = (header*)Allocator::malloc(size*sizeof(value_type) + sizeof(header));

        if (!tmp)
            bad_alloc();

        tmp[0] = header(size);
        ++tmp;
        m_ptr = (value_type*)tmp;
    };
};

template<class value_type, int n_elem, class Allocator>
void stack_array<value_type,n_elem,Allocator>::reserve(size_t new_size)
{
    if (new_size <= n_elem)
        return;

    header* tmp     = (header*)m_ptr;
    --tmp;

    size_t m_size   = tmp[0].m_size;
    tmp             = (header*)Allocator::realloc(tmp, 
                        m_size*sizeof(value_type) + sizeof(header),
                        new_size * sizeof(value_type) + sizeof(header));

    if (!tmp)
        bad_alloc();

    tmp[0]  = header(new_size);
    ++tmp;
    m_ptr   = (value_type*)tmp;
};

template<class value_type, int n_elem,class Allocator>
stack_array<value_type,n_elem,Allocator>::~stack_array()
{
    if (m_destructor)
        m_destructor->destroy(get());

    if (m_ptr)
    {
        header* tmp = (header*)m_ptr;
        --tmp;
        Allocator::free(tmp, tmp[0].m_size*sizeof(value_type) + sizeof(header));
    };
};

template<class value_type, int n_elem,class Allocator>
void stack_array<value_type,n_elem,Allocator>::bad_alloc() const
{
    throw std::bad_alloc();
}

};};
