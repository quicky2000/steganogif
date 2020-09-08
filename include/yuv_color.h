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

#ifndef STEGANOGIF_YUV_COLOR_H
#define STEGANOGIF_YUV_COLOR_H

#include "my_color.h"
#include <cmath>
#include <iomanip>

namespace steganogif
{
    class yuv_color
    {
        friend
        std::ostream & operator<<(std::ostream & p_stream,const yuv_color & p_color);

      public:

        inline
        yuv_color(const lib_bmp::my_color & p_color);

        inline
        yuv_color( float p_y
                 , float p_u
                 , float p_v
                 );

        inline
        float get_y() const;

        inline
        float get_u() const;

        inline
        float get_v() const;

        bool operator==(const yuv_color & p_color)const;

        bool operator<(const yuv_color & p_color)const;

        lib_bmp::my_color to_rgb_color() const;

      private:

        inline static
        uint8_t normalise(long p_value);

        float m_y;
        float m_u;
        float m_v;

    };

    //-------------------------------------------------------------------------
    yuv_color::yuv_color(const lib_bmp::my_color & p_color)
    : m_y{(float)(0.299 * p_color.get_red() + 0.587 * p_color.get_green() + 0.114 * p_color.get_blue())}
    , m_u{(float)(-0.14713 * p_color.get_red() - 0.28886 * p_color.get_green() + 0.436 * p_color.get_blue())}
    , m_v{(float)(0.615 * p_color.get_red() - 0.51498 * p_color.get_green() - 0.10001 * p_color.get_blue())}
    {
    }

    //-------------------------------------------------------------------------
    float
    yuv_color::get_y() const
    {
        return m_y;
    }

    //-------------------------------------------------------------------------
    float
    yuv_color::get_u() const
    {
        return m_u;
    }

    //-------------------------------------------------------------------------
    float
    yuv_color::get_v() const
    {
        return m_v;
    }

    //----------------------------------------------------------------------------
    bool yuv_color::operator==(const yuv_color & p_color)const
    {
        return m_y == p_color.m_y && m_u == p_color.m_u && m_v == p_color.m_v;
    }

    //----------------------------------------------------------------------------
    bool yuv_color::operator <(const yuv_color & p_color)const
    {
        if(m_y < p_color.m_y)
        {
            return true;
        }
        else if(m_y > p_color.m_y)
        {
            return false;
        }
        else if(m_u < p_color.m_u)
        {
            return true;
        }
        else if(m_u > p_color.m_u)
        {
            return false;
        }
        else if(m_v < p_color.m_v)
        {
            return true;
        }
        return false;
    }

    //-------------------------------------------------------------------------
    std::ostream &
    operator<<(std::ostream & p_stream,const yuv_color & p_color)
    {
        p_stream << "Y=" << std::setw(3) << p_color.m_y << ", U=" << std::setw(3) << p_color.m_u << ", V=" << std::setw(3) << p_color.m_v ;
        return p_stream;
    }

    //-------------------------------------------------------------------------
    lib_bmp::my_color
    yuv_color::to_rgb_color() const
    {
        uint8_t l_red = normalise(std::lround(m_y + 1.13983 * m_v));
        uint8_t l_green = normalise(std::lround(m_y - 0.39465 * m_u - 0.58060 * m_v));
        uint8_t l_blue = normalise(std::lround(m_y + 2.03211 * m_u));
        return lib_bmp::my_color(l_red, l_green, l_blue);
    }

    //-------------------------------------------------------------------------
    uint8_t
    yuv_color::normalise(long p_value)
    {
        if(p_value <= 255 && p_value >=0)
        {
            return p_value;
        }
        else if(p_value > 255)
        {
            return 255;
        }
        return 0;
    }

    //-------------------------------------------------------------------------
    yuv_color::yuv_color(float p_y,
                         float p_u,
                         float p_v
                        )
                        : m_y(p_y)
                        , m_u(p_u)
                        , m_v(p_v)
    {

    }
}
#endif //STEGANOGIF_YUV_COLOR_H
// EOF