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

#ifndef STEGANOGIF_SPLITTABLE_H
#define STEGANOGIF_SPLITTABLE_H

#include "countable_item.h"

#include <set>
#include <map>
#include <limits>

namespace steganogif
{
    template<typename T>
    class splittable;

    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splittable<T> & p_splittable);

    template<typename T>
    bool operator<( const splittable<T> & p_splittable1
                  , const splittable<T> & p_splittable2
                  );

    template<typename T>
    class splittable
    {
        friend
        std::ostream & operator<< <>(std::ostream & p_stream, const splittable & p_splittable);

        friend
        bool operator< <>( const splittable & p_splittable1
                         , const splittable & p_splittable2
                         );
      public:
        inline
        splittable(const std::set<countable_item<T>> & p_items);

        inline
        splittable(const std::map<T, unsigned int> & p_items);

        inline
        splittable();

        inline
        unsigned int get_nb() const;

        inline
        void add_item(const countable_item<T> & p_item);

        inline
        bool is_splittable() const;

        inline
        std::pair<splittable, splittable> split() const;

        inline
        T get_first_value() const;

        inline
        T get_last_value() const;

        inline
        const countable_item<T> & get_first() const;

        inline
        const countable_item<T> & get_last() const;

        inline
        bool contains(const T & p_value) const;

        inline
        T get_most_frequent() const;

        inline
        T get_average() const;

        inline
        void remove(const countable_item<T> & p_value);

        inline
        splittable<T>
        remove_extrema() const;

      private:
        std::set<countable_item<T>> m_items;
        unsigned int m_nb;
    }
    ;

    //------------------------------------------------------------------------------
    template<typename T>
    splittable<T>::splittable(const std::set<countable_item<T>> & p_items)
    : m_items(p_items)
    , m_nb(0)
    {
        for(auto l_iter: p_items)
        {
            m_nb += l_iter.get_nb();
        }
    }

    //------------------------------------------------------------------------------
    template<typename T>
    splittable<T>::splittable(const std::map<T, unsigned int> & p_items)
    : m_nb(0)
    {
        for(auto l_iter: p_items)
        {
            m_items.insert(countable_item(l_iter.first, l_iter.second));
            m_nb += l_iter.second;
        }

    }

    //------------------------------------------------------------------------------
    template<typename T>
    splittable<T>::splittable()
    : m_nb(0)
    {
    }

    //------------------------------------------------------------------------------
    template<typename T>
    void splittable<T>::add_item(const countable_item<T> & p_item)
    {
        m_items.insert(p_item);
        m_nb += p_item.get_nb();
    }

    //------------------------------------------------------------------------------
    template<typename T>
    unsigned int splittable<T>::get_nb() const
    {
        return m_nb;
    }

    //------------------------------------------------------------------------------
    template<typename T>
    bool splittable<T>::is_splittable() const
    {
        return m_items.size() > 1;
    }

    //------------------------------------------------------------------------------
    template<typename T>
    std::pair<splittable<T>, splittable<T>> splittable<T>::split() const
    {
        assert(is_splittable());
        splittable l_splittable1;
        splittable l_splittable2;
        splittable * l_target = & l_splittable1;
        for(auto l_iter: m_items)
        {
            if(!l_splittable2.get_nb())
            {
                size_t l_nb2 = m_nb - l_splittable1.get_nb();
                if(std::abs((int)(2 * (l_splittable1.get_nb() + l_iter.get_nb()) - m_nb)) > std::abs((int)(l_splittable1.get_nb() - l_nb2)))
                {
                    l_target = & l_splittable2;
                }
            }
            l_target->add_item(l_iter);
        }
        return std::make_pair(l_splittable1, l_splittable2);
    }

    //------------------------------------------------------------------------------
    template<typename T>
    bool operator<( const splittable<T> & p_splittable1
                  , const splittable<T> & p_splittable2
                  )
    {
        if(p_splittable1.m_nb != p_splittable2.m_nb)
        {
            return p_splittable1.m_nb < p_splittable2.m_nb;
        }
        else
        {
            return p_splittable1.m_items.size() < p_splittable2.m_items.size();
        }
    }

    //------------------------------------------------------------------------------
    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const splittable<T> & p_splittable)
    {
        p_stream << "Nb = " << p_splittable.m_nb << " {";
        for(auto l_iter: p_splittable.m_items)
        {
            p_stream << "{" << l_iter << "} ";
        }
        p_stream << "}";
        return p_stream;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    splittable<T>::get_first_value() const
    {
        assert(m_items.size());
        return m_items.begin()->get_value();
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    splittable<T>::get_last_value() const
    {
        assert(m_items.size());
        return m_items.rbegin()->get_value();
    }

    //-------------------------------------------------------------------------
    template <typename T>
    bool
    splittable<T>::contains(const T & p_value) const
    {
        for(auto l_iter: m_items)
        {
            if(p_value == l_iter.get_value())
            {
                return true;
            }
        }
        return false;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    splittable<T>::get_most_frequent() const
    {
        unsigned int l_max_count = std::numeric_limits<unsigned int>::min();
        T l_value;
        for(auto l_iter: m_items)
        {
            if(l_iter.get_nb() > l_max_count)
            {
                l_max_count = l_iter.get_nb();
                l_value = l_iter.get_value();
            }
        }
        return l_value;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    splittable<T>::get_average() const
    {
        unsigned int l_total = 0;
        unsigned int l_total_nb = 0;
        for(auto l_iter: m_items)
        {
            l_total += l_iter.get_value() * l_iter.get_nb();
            l_total_nb += l_iter.get_nb();
        }
        return l_total / l_total_nb;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    void
    splittable<T>::remove(const countable_item<T> & p_value)
    {
        m_nb -= p_value.get_nb();
        m_items.erase(p_value);
    }

    //-------------------------------------------------------------------------
    template<typename T>
    splittable<T>
    splittable<T>::remove_extrema() const
    {
        splittable<T> l_result{m_items};
        l_result.remove(*m_items.begin());
        l_result.remove(*m_items.rbegin());
        return l_result;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    const countable_item<T> &
    splittable<T>::get_first() const
    {
        return *m_items.begin();
    }


    //-------------------------------------------------------------------------
    template <typename T>
    const countable_item<T> &
    splittable<T>::get_last() const
    {
        return *m_items.rbegin();
    }

}
#endif //STEGANOGIF_SPLITTABLE_H
