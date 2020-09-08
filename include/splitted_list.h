/*
      This file is part of steganogif
      Copyright (C) 2020  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef STEGANOGIF_SPLITTED_LIST_H
#define STEGANOGIF_SPLITTED_LIST_H

#include "splittable_list.h"
#include <vector>
#if __cplusplus >= 201703L
#if (!defined(MACX))
#include <optional>
#else // (!defined(MACX))
#include <experimental/optional>
#endif // (!defined(MACX))
#endif // __cplusplus >= 201703L

namespace steganogif
{
    template<typename T>
    class splitted_list;

    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splitted_list<T> & p_splittable_list);

    template<typename T>
    class splitted_list
    {

        friend
        std::ostream & operator<< <>(std::ostream & p_stream, const splitted_list & p_splitted_list);

      public:
        inline
        splitted_list(const std::vector<splittable<T>> & p_list);

        inline
#if (!defined(MACX))
        std::optional<unsigned int>
#else // (!defined(MACX))
        std::experimental::optional<unsigned int>
#endif // (!defined(MACX))
        get_index(const T & p_value) const;

        inline
        void compute_most_frequent();

        inline
        void compute_average();

        inline
        T get_most_frequent(const T & p_value) const;

        inline
        T get_average(const T & p_value) const;

      private:

        std::vector<splittable<T>> m_list;
        std::vector<T> m_most_frequents;
        std::vector<T> m_average;
    };

    //-------------------------------------------------------------------------
    template<typename T>
    splitted_list<T>::splitted_list(const std::vector<splittable<T>> & p_list)
    : m_list{p_list}
    {

    }

    //-------------------------------------------------------------------------
    template <typename T>
#if (!defined(MACX))
std::optional<unsigned int>
#else // (!defined(MACX))
std::experimental::optional<unsigned int>
#endif // (!defined(MACX))
    splitted_list<T>::get_index(const T & p_value) const
    {
        for(unsigned int l_index = 0; l_index < m_list.size(); ++l_index)
        {
            if(m_list[l_index].contains(p_value))
            {
                return l_index;
            }
        }
        return {};
    }

    //-------------------------------------------------------------------------
    template<typename T>
    void splitted_list<T>::compute_most_frequent()
    {
        if(!m_most_frequents.empty())
        {
            throw quicky_exception::quicky_logic_exception("Most frequent values already computed", __LINE__, __FILE__);
        }
        for(auto l_iter: m_list)
        {
            m_most_frequents.emplace_back(l_iter.get_most_frequent());
        }
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    steganogif::splitted_list<T>::get_most_frequent(const T & p_value) const
    {
#if (!defined(MACX))
        std::optional<unsigned int>
#else // (!defined(MACX))
        std::experimental::optional<unsigned int>
#endif // (!defined(MACX))
        l_index = get_index(p_value);
        if(l_index)
        {
            assert(l_index < m_most_frequents.size());
            return m_most_frequents[l_index.value()];
        }
        std::stringstream l_stream;
        l_stream << p_value;
        throw quicky_exception::quicky_logic_exception("Value " + l_stream.str() + " not found for most frequent", __LINE__, __FILE__);
    }

    //-------------------------------------------------------------------------
    template<typename T>
    void splitted_list<T>::compute_average()
    {
        if(!m_average.empty())
        {
            throw quicky_exception::quicky_logic_exception("Average values already computed", __LINE__, __FILE__);
        }
        for(auto l_iter: m_list)
        {
            m_average.emplace_back(l_iter.get_most_frequent());
        }
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    steganogif::splitted_list<T>::get_average(const T & p_value) const
    {
#if (!defined(MACX))
        std::optional<unsigned int>
#else // (!defined(MACX))
        std::experimental::optional<unsigned int>
#endif // (!defined(MACX))
        l_index = get_index(p_value);
        if(l_index)
        {
            assert(l_index < m_average.size());
            return m_average[l_index.value()];
        }
        std::stringstream l_stream;
        l_stream << p_value;
        throw quicky_exception::quicky_logic_exception("Value " + l_stream.str() + " not found for average", __LINE__, __FILE__);
    }

    //------------------------------------------------------------------------------
    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splitted_list<T> & p_splitted_list)
    {
        p_stream << "{" << std::endl;
        for(auto l_iter: p_splitted_list.m_list)
        {
            p_stream << l_iter << " " << std::endl;
        }
        p_stream << "}";
        return p_stream;
    }


}
#endif //STEGANOGIF_SPLITTED_LIST_H
// EOF