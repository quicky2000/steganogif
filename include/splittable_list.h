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

#ifndef STEGANOGIF_SPLITTABLE_LIST_H
#define STEGANOGIF_SPLITTABLE_LIST_H

#include "splittable.h"
#include <tuple>
#include <vector>
#include <algorithm>

namespace steganogif
{
    template<typename T>
    class splittable_list;

    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splittable_list<T> & p_splittable_list);

    template<typename T>
    class splittable_list
    {
        friend
        std::ostream & operator<< <>(std::ostream & p_stream, const splittable_list & p_splittable_list);

      public:
        inline
        splittable_list(const splittable<T> & p_splittable);

        inline
        void split(unsigned int p_number);

        inline
        std::vector<splittable<T>> to_vector();

      private:
        std::multiset<splittable<T>> m_splittables;
    };

    //------------------------------------------------------------------------------
    template<typename T>
    splittable_list<T>::splittable_list(const splittable<T> & p_splittable)
    {
        m_splittables.insert(p_splittable);
    }

    //------------------------------------------------------------------------------
    template<typename T>
    void splittable_list<T>::split(unsigned int p_number)
    {
        if(1 == m_splittables.size())
        {
            splittable<T> l_first;
            l_first.add_item(m_splittables.begin()->get_first());
            splittable<T> l_last;
            l_last.add_item(m_splittables.begin()->get_last());
            splittable<T> l_intermediate = m_splittables.begin()->remove_extrema();
            m_splittables.clear();
            m_splittables.insert(l_first);
            m_splittables.insert(l_intermediate);
            m_splittables.insert(l_last);
        }
        while(m_splittables.size() < p_number)
        {
            auto l_iter = m_splittables.rbegin();
            for(; l_iter != m_splittables.rend() && !l_iter->is_splittable(); ++l_iter)
            {
            }
            if(l_iter->is_splittable())
            {
                splittable<T> l_items1, l_items2;
                std::tie(l_items1, l_items2) = l_iter->split();
                m_splittables.erase(*l_iter);
                m_splittables.insert(l_items1);
                m_splittables.insert(l_items2);
            }
            else
            {
                break;
            }
        }
    }

    //------------------------------------------------------------------------------
    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splittable_list<T> & p_splittable_list)
    {
        p_stream << "{" << std::endl;
        for(auto l_iter: p_splittable_list.m_splittables)
        {
            p_stream << l_iter << " " << std::endl;
        }
        p_stream << "}";
        return p_stream;
    }

    //------------------------------------------------------------------------------
    template <typename T>
    std::vector<splittable<T>>
    splittable_list<T>::to_vector()
    {
        std::vector<splittable<T>> l_vector;
        for(auto l_iter: m_splittables)
        {
            l_vector.emplace_back(l_iter);
        }
        auto l_f = [](const splittable<T> & p_1, const splittable<T> & p_2)
        {
            return p_1.get_first_value() < p_2.get_first_value();
        };
        std::sort(l_vector.begin(), l_vector.end(), l_f);
        return l_vector;
    }

}
#endif //STEGANOGIF_SPLITTABLE_LIST_H
