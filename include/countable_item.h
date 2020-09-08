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

#ifndef STEGANOGIF_COUNTABLE_ITEM_H
#define STEGANOGIF_COUNTABLE_ITEM_H

#include <iostream>

namespace steganogif
{

    template<typename T>
    class countable_item;

    template<typename T>
    std::ostream & operator<<(std::ostream & p_stream, const countable_item<T> & p_item);

    template<typename T>
    bool operator<( const countable_item<T> & p_item1
                  , const countable_item<T> & p_item2
                  );

    template<typename T>
    class countable_item
    {
        friend
        std::ostream & operator<< <>(std::ostream & p_stream, const countable_item & p_item);

        friend
        bool operator< <>( const countable_item & p_item1
                         , const countable_item & p_item2
                         );
      public:

        inline
        countable_item( T p_value
                      , unsigned int p_nb
                      );

        inline
        T get_value() const;

        inline
        unsigned int get_nb() const;

      private:
        T m_value;
        unsigned int m_nb;
    };

    //------------------------------------------------------------------------------
    template<typename T>
    countable_item<T>::countable_item( T p_value
                                     , unsigned int p_nb
                                     )
                                     : m_value(p_value)
                                     , m_nb(p_nb)
    {
    }

    //------------------------------------------------------------------------------
    template<typename T>
    unsigned int countable_item<T>::get_nb() const
    {
        return m_nb;
    }

    //------------------------------------------------------------------------------
    template<typename T>
    bool operator<( const countable_item<T> & p_item1
                  , const countable_item<T> & p_item2
                  )
    {
        return p_item1.m_value < p_item2.m_value;
    }

    //------------------------------------------------------------------------------
    template<typename T>
    std::ostream & operator<<( std::ostream & p_stream
                             , const countable_item<T> & p_item
                             )
    {
        p_stream << "V = [" << p_item.m_value << "] Nb = " << p_item.m_nb;
        return p_stream;
    }

    //------------------------------------------------------------------------------
    template <typename T>
    T
    countable_item<T>::get_value() const
    {
        return m_value;
    }

}
#endif //STEGANOGIF_COUNTABLE_ITEM_H
// EOF