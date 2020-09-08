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

#ifndef STEGANOGIF_H
#define STEGANOGIF_H

#include "sha1.h"
#include "my_bmp.h"
#include "yuv_color.h"
#include "splittable_list.h"
#include "splitted_list.h"
#include <string>
#include <chrono>
#include <random>
#include <set>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <map>
#include <sstream>

namespace steganogif
{
    class steganogif
    {
      public:
        inline
        steganogif(const std::string & p_password);

        inline
        ~steganogif();

        inline
        void encode();

      private:

        inline
        lib_bmp::my_bmp
        compute_simplified_bmp(const lib_bmp::my_bmp & p_bmp);

        inline
        std::map<lib_bmp::my_color, lib_bmp::my_color>
        compute_simplified_colors( const lib_bmp::my_bmp & p_bmp);

        inline
        std::map<lib_bmp::my_color, lib_bmp::my_color>
        compute_simplified_colors( const std::set<lib_bmp::my_color> & p_all_colors
                                 , const std::map<uint8_t, unsigned int> & p_red_colors
                                 , const std::map<uint8_t, unsigned int> & p_green_colors
                                 , const std::map<uint8_t, unsigned int> & p_blue_colors
                                 );

        inline
        std::map<lib_bmp::my_color, lib_bmp::my_color>
        compute_simplified_colors( const std::set<lib_bmp::my_color> & p_all_colors
                                 , const std::map<int, unsigned int> & p_y_colors
                                 , const std::map<int, unsigned int> & p_u_colors
                                 , const std::map<int, unsigned int> & p_v_colors
                                 );

        /**
         * Compute geometric distance between 2 colors
         * @param p_color1
         * @param p_color2
         * @return
         */
        inline static
        float dist( const lib_bmp::my_color & p_color1
                  , const lib_bmp::my_color & p_color2
                  );

        /**
         * Compute geometric distance between 2 colors
         * @param p_color1
         * @param p_color2
         * @return
         */
        inline static
        float dist( const yuv_color & p_color1
                  , const yuv_color & p_color2
                  );

        std::seed_seq * m_seed;
    };

    //-------------------------------------------------------------------------
    steganogif::steganogif(const std::string & p_password)
    : m_seed(nullptr)
    {
        sha1 l_sha1{(const uint8_t*)p_password.data(), p_password.size()};

        // Generate seed from password hash
        m_seed = new std::seed_seq({ l_sha1.get_key(0)
                                   , l_sha1.get_key(1)
                                   , l_sha1.get_key(2)
                                   , l_sha1.get_key(3)
                                   , l_sha1.get_key(4)
                                   }
                                  );
    }

    //-------------------------------------------------------------------------
    steganogif::~steganogif()
    {
        delete m_seed;
    }

    //-------------------------------------------------------------------------
    void
    steganogif::encode()
    {
        std::mt19937 l_generator{*m_seed};

        // BMP file
        lib_bmp::my_bmp l_bmp("/media/data/Programmation/C++/Git_repositories/steganogif/test/20160729_095841.bmp");

        lib_bmp::my_bmp * l_work_bmp = &l_bmp;

        if(l_bmp.get_nb_bits_per_pixel() > 8)
        {
            l_work_bmp = new lib_bmp::my_bmp(compute_simplified_bmp(l_bmp));
            std::array<uint8_t, 4> l_red_components{0,64,128,255};
            std::array<uint8_t, 8> l_green_components{0, 32, 64, 96, 128, 160, 192, 255};
            unsigned int l_index = 0;
            for(auto l_iter_red: l_red_components)
            {
                for(auto l_iter_green: l_green_components)
                {
                    for(auto l_iter_blue: l_red_components)
                    {
                        l_work_bmp->get_palette().set_color(lib_bmp::my_color_alpha(l_iter_red, l_iter_green, l_iter_blue), l_index);
                        ++l_index;
                    }
                }
            }
            // obtain a seed from the system clock:
            std::mt19937 l_color_generator{(unsigned int)std::chrono::system_clock::now().time_since_epoch().count()};
            for(;l_index < 256; ++l_index)
            {
                unsigned int l_componant_index = l_color_generator() % 3;
                lib_bmp::my_color_alpha l_original_color = l_work_bmp->get_palette().get_color(l_index - 128);
                std::cout << "[" << l_index << "] " << l_original_color << " => " ;
                unsigned int l_componant = (0 ==l_componant_index ) ? l_original_color.get_red() : ((1 == l_componant_index) ? l_original_color.get_green() : l_original_color.get_blue());
                unsigned int l_offset = (1 + (l_color_generator() % 14));
                l_componant = l_componant != 255 ? l_componant + l_offset : l_componant - l_offset;
                switch (l_componant_index)
                {
                    case 0:
                        l_original_color.set_red(l_componant);
                        break;
                    case 1:
                        l_original_color.set_green(l_componant);
                        break;
                    case 2:
                        l_original_color.set_blue(l_componant);
                        break;
                    default:
                        throw quicky_exception::quicky_logic_exception("Unknowm componant index : " + std::to_string(l_componant_index), __LINE__, __FILE__);
                }
                std::cout << l_original_color << std::endl ;
                l_work_bmp->get_palette().set_color(l_original_color, l_index);
            }
            l_work_bmp->save("simplified.bmp");
        }

        // Check there are no duplicated colors in palette
        std::map<lib_bmp::my_color, unsigned int> l_colors;
        for(unsigned int l_index = 0; l_index < l_work_bmp->get_palette().get_size(); ++l_index)
        {
            //std::cout << l_bmp_256.get_palette().get_color(l_index) << std::endl;
            if(l_colors.count(l_work_bmp->get_palette().get_color(l_index)))
            {
                std::stringstream l_color_stream;
                l_color_stream << l_work_bmp->get_palette().get_color(l_index);
                throw quicky_exception::quicky_logic_exception("Color duplicated at index " + std::to_string(l_index) + " / " + std::to_string(l_colors.find(l_work_bmp->get_palette().get_color(l_index))->second) + " : " + l_color_stream.str(), __LINE__, __FILE__);
            }
            l_colors.insert(std::make_pair(l_work_bmp->get_palette().get_color(l_index), l_index));
        }

        std::cout << "Result : " << l_colors.size() << std::endl;

//#if 0
        while(l_colors.size())
        {
            std::cout << l_colors.size() << std::endl;
            float l_min = std::numeric_limits<float>::max();
            lib_bmp::my_color l_lower_color;
            lib_bmp::my_color l_upper_color;
            for(auto l_iter: l_colors)
            {
                auto l_search_min = [=, &l_min, &l_lower_color, &l_upper_color](const auto p_color)
                {
                    if (p_color != l_iter)
                    {
                        float l_dist = dist(p_color.first, l_iter.first);
                        if (l_dist < l_min)
                        {
                            l_min = l_dist;
                            l_lower_color = p_color.first;
                            l_upper_color = l_iter.first;
                        }
                    }
                };
                std::for_each(l_colors.begin(), l_colors.end(), l_search_min);
            }
            std::cout << l_upper_color << " <==> " << l_lower_color << " : " << l_min << std::endl;
            l_colors.erase(l_upper_color);
            l_colors.erase(l_lower_color);
        }
        for(unsigned int l_index = 0; l_index < 10; ++l_index)
        {
            std::cout << l_generator() << std::endl;
        }
//#endif // 0
        if(l_work_bmp != &l_bmp)
        {
            delete l_work_bmp;
        }
    }

    //-------------------------------------------------------------------------
    float steganogif::dist( const lib_bmp::my_color & p_color1
                          , const lib_bmp::my_color & p_color2
                          )
    {
        float l_red_diff = ((float) p_color1.get_red()) - ((float) p_color2.get_red());
        float l_green_diff = ((float) p_color1.get_green()) - ((float) p_color2.get_green());
        float l_blue_diff = ((float) p_color1.get_blue()) - ((float) p_color2.get_blue());
        float l_dist = sqrt(l_red_diff * l_red_diff + l_green_diff * l_green_diff + l_blue_diff * l_blue_diff);
        return l_dist;
    }

    //-------------------------------------------------------------------------
    float
    steganogif::dist( const yuv_color & p_color1
                    , const yuv_color & p_color2
                    )
    {
        float l_y_diff = ((float) p_color1.get_y()) - ((float) p_color2.get_y());
        float l_u_diff = ((float) p_color1.get_u()) - ((float) p_color2.get_u());
        float l_v_diff = ((float) p_color1.get_v()) - ((float) p_color2.get_v());
        float l_dist = sqrt(l_y_diff * l_y_diff + l_u_diff * l_u_diff + l_v_diff * l_v_diff);
        return l_dist;
    }

    //-------------------------------------------------------------------------
    std::map<lib_bmp::my_color, lib_bmp::my_color>
    steganogif::compute_simplified_colors(const std::set<lib_bmp::my_color> & p_all_colors,
                                          const std::map<uint8_t, unsigned int> & p_red_colors,
                                          const std::map<uint8_t, unsigned int> & p_green_colors,
                                          const std::map<uint8_t, unsigned int> & p_blue_colors
                                         )
    {
        splittable_list<uint8_t> l_list_r(p_red_colors);
        l_list_r.split(16);
        splitted_list<uint8_t> l_splitted_list_r{l_list_r.to_vector()};
        l_splitted_list_r.compute_average();

        splittable_list<uint8_t> l_list_g(p_green_colors);
        l_list_g.split(8);
        splitted_list<uint8_t> l_splitted_list_g{l_list_g.to_vector()};
        l_splitted_list_g.compute_average();

        splittable_list<uint8_t> l_list_b(p_blue_colors);
        l_list_b.split(6);
        splitted_list<uint8_t> l_splitted_list_b{l_list_b.to_vector()};
        l_splitted_list_b.compute_average();

        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance;

        for(auto l_iter: p_all_colors)
        {
            l_color_correspondance[l_iter] = lib_bmp::my_color( l_splitted_list_r.get_average(l_iter.get_red())
                                                              , l_splitted_list_g.get_average(l_iter.get_green())
                                                              , l_splitted_list_b.get_average(l_iter.get_blue())
                                                              );
        }

        return l_color_correspondance;
    }

    //-------------------------------------------------------------------------
    std::map<lib_bmp::my_color, lib_bmp::my_color>
    steganogif::compute_simplified_colors(const std::set<lib_bmp::my_color> & p_all_colors,
                                          const std::map<int, unsigned int> & p_y_colors,
                                          const std::map<int, unsigned int> & p_u_colors,
                                          const std::map<int, unsigned int> & p_v_colors
                                         )
    {
        // Split color component lists to reduce the number of colors

        splittable_list<int> l_list_y(p_y_colors);
        l_list_y.split(8);
        splitted_list<int> l_splitted_list_y{l_list_y.to_vector()};
        l_splitted_list_y.compute_average();

        splittable_list<int> l_list_u(p_u_colors);
        l_list_u.split(4);
        splitted_list<int> l_splitted_list_u{l_list_u.to_vector()};
        std::cout << l_splitted_list_u << std::endl;
        l_splitted_list_u.compute_average();

        splittable_list<int> l_list_v(p_v_colors);
        l_list_v.split(8);
        splitted_list<int> l_splitted_list_v{l_list_v.to_vector()};
        std::cout << l_splitted_list_v << std::endl;
        l_splitted_list_v.compute_average();

        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance;

        for(auto l_iter: p_all_colors)
        {
            yuv_color l_yuv{l_iter};
            yuv_color l_translated_yuv{ (float)l_splitted_list_y.get_average(l_yuv.get_y())
                                      , (float)l_splitted_list_u.get_average(l_yuv.get_u())
                                      , (float)l_splitted_list_v.get_average(l_yuv.get_v())
                                      };
            l_color_correspondance[l_iter] = l_translated_yuv.to_rgb_color();
        }
        return l_color_correspondance;
    }

    //-------------------------------------------------------------------------
    lib_bmp::my_bmp
    steganogif::compute_simplified_bmp(const lib_bmp::my_bmp & p_bmp)
    {
        lib_bmp::my_bmp l_new_bmp(p_bmp.get_width(), p_bmp.get_height(), 8);
        for(unsigned int l_y = 0; l_y < p_bmp.get_height(); ++l_y)
        {
            for (unsigned int l_x = 0; l_x < p_bmp.get_width(); ++l_x)
            {
                l_new_bmp.set_pixel_color(l_x, l_y, p_bmp.get_pixel_color(l_x, l_y));
            }
        }
        return l_new_bmp;
    }

    //-------------------------------------------------------------------------
    std::map<lib_bmp::my_color, lib_bmp::my_color>
    steganogif::compute_simplified_colors(const lib_bmp::my_bmp & p_bmp)
    {
        // Collect color stats
        std::set<lib_bmp::my_color> l_all_colors;
        std::map<uint8_t, unsigned int> l_red_colors;
        std::map<uint8_t, unsigned int> l_green_colors;
        std::map<uint8_t, unsigned int> l_blue_colors;
        std::map<int, unsigned int> l_y_colors;
        std::map<int, unsigned int> l_u_colors;
        std::map<int, unsigned int> l_v_colors;
        std::map<yuv_color, unsigned int> l_all_yuv_colors;
        for(unsigned int l_y = 0; l_y < p_bmp.get_height(); ++l_y)
        {
            for(unsigned int l_x = 0; l_x < p_bmp.get_width(); ++l_x)
            {
                lib_bmp::my_color l_color = p_bmp.get_pixel_color(l_x, l_y);
                l_all_colors.insert(l_color);
                l_red_colors[l_color.get_red()]++;
                l_green_colors[l_color.get_green()]++;
                l_blue_colors[l_color.get_blue()]++;
                yuv_color l_yuv_color{l_color};
                l_y_colors[(int)l_yuv_color.get_y()]++;
                l_u_colors[(int)l_yuv_color.get_u()]++;
                l_v_colors[(int)l_yuv_color.get_v()]++;
                l_all_yuv_colors[l_yuv_color]++;
            }
        }
        std::cout << "All colors : " << l_all_colors.size() << std::endl;
        std::cout << "Red colors : " << l_red_colors.size() << std::endl;
        std::cout << "Green colors : " << l_green_colors.size() << std::endl;
        std::cout << "Blue colors : " << l_blue_colors.size() << std::endl;
        std::cout << "YUV colors : " << l_all_yuv_colors.size() << std::endl;
        std::cout << "Y colors : " << l_y_colors.size() << std::endl;
        std::cout << "U colors : " << l_u_colors.size() << std::endl;
        std::cout << "V colors : " << l_v_colors.size() << std::endl;

        // Compute simplified colors
        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance = compute_simplified_colors(l_all_colors, l_red_colors, l_green_colors, l_blue_colors);

        // Count number of simplified colors
        std::set<lib_bmp::my_color> l_new_colors;
        for(auto l_iter: l_color_correspondance)
        {
            l_new_colors.insert(l_iter.second);
        }
        std::cout << "Number of translated colors : " << l_new_colors.size() << std::endl;

        return l_color_correspondance;
    }

}
#endif // STEGANOGIF_H
// EOF