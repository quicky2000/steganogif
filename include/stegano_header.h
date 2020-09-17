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

#ifndef STEGANOGIF_STEGANO_HEADER_H
#define STEGANOGIF_STEGANO_HEADER_H

#include "quicky_exception.h"
#include <cinttypes>
#include <vector>

namespace steganogif
{
    class stegano_header
    {
      public:
        inline
        stegano_header(uint32_t p_content_size);

        inline
        stegano_header(std::vector<uint8_t> & p_content);

        /**
         * Return size of header in byte once encoded
         * @return encoded size
         */
        inline
        unsigned int get_size() const;

        /**
         * Encode header content in a vector of byte
         * @return encoded content of header
         */
        inline
        std::vector<uint8_t> encode() const;

      private:

        /**
         * Encode 32 bits value and push back produced result to p_content
         * @param p_value value to encode
         * @param p_content receive encoded value
         */
        inline static
        void encode_and_add( uint32_t p_value
                           , std::vector<uint8_t> & p_content
                           );

        /**
         * Decode value and remove it from content
         * throw and exception in case of undecodable content
         * @param p_content content
         * @return decoded value
         */
        inline static
        uint32_t decode_and_remove(std::vector<uint8_t> & p_content);

        /**
         * Version number
         */
        uint32_t m_version = 0;

        /**
         * Size of content hidden in GIF
         */
        uint32_t m_content_size;
    };

    //-------------------------------------------------------------------------
    stegano_header::stegano_header(uint32_t p_content_size)
    : m_content_size(p_content_size)
    {

    }

    //-------------------------------------------------------------------------
    unsigned int
    stegano_header::get_size() const
    {
        return m_content_size;
    }

    //-------------------------------------------------------------------------
    std::vector<uint8_t>
    stegano_header::encode() const
    {
        std::vector<uint8_t> l_content;
        encode_and_add(m_version, l_content);
        encode_and_add(m_content_size, l_content);
        return l_content;
    }

    void
    stegano_header::encode_and_add(uint32_t p_value,
                                   std::vector<uint8_t> & p_content
                                  )
    {
        do
        {
            uint8_t l_7_bits = (uint8_t)(p_value & 0x7F);
            p_value = p_value >> 7;
            if(p_value)
            {
                l_7_bits |= 0x80;
            }
            p_content.emplace_back(l_7_bits);
        } while(p_value);
    }

    //-------------------------------------------------------------------------
    uint32_t
    stegano_header::decode_and_remove(std::vector<uint8_t> & p_content)
    {
        uint32_t l_result = 0;
        uint32_t l_shift = 0;
        while(p_content.size())
        {
            uint32_t l_byte = p_content[0];
            p_content.erase(p_content.begin());
            l_result |= (l_byte & 0x7F) << l_shift;
            if(l_byte & 0x80)
            {
                l_shift += 7;
                if(l_shift > 28)
                {
                    throw quicky_exception::quicky_logic_exception("Bad encoded value: too big", __LINE__, __FILE__);
                }
            }
            else
            {
                return l_result;
            }
        }
        throw quicky_exception::quicky_logic_exception("Bad encoded value: too short", __LINE__, __FILE__);
    }

    //-------------------------------------------------------------------------
    stegano_header::stegano_header(std::vector<uint8_t> & p_content)
    : m_version(decode_and_remove(p_content))
    , m_content_size(decode_and_remove(p_content))
    {
        if(0 != m_version)
        {
            throw quicky_exception::quicky_logic_exception("Bad header version", __LINE__, __FILE__);
        }
    }

}
#endif //STEGANOGIF_STEGANO_HEADER_H
// EOF