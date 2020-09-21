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
#include "stegano_header.h"
#include "gif_streamer.h"
#include "gif.h"
#include "gif_graphic_block.h"
#include <string>
#include <chrono>
#include <random>
#include <set>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <map>
#include <sstream>
#include <fstream>

#if __has_include(<filesystem>)
#include <filesystem>
#endif // filesystem

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
        void encode( const std::string & p_output_file_name
                   , const std::string & p_content_file_name
                   , const std::string & p_transport_file_name
                   );

        inline
        void decode( const std::string & p_input_file_name
                   , const std::string & p_content_file_name
                   );

      private:

        inline
        lib_bmp::my_bmp
        compute_simplified_bmp(const lib_bmp::my_bmp & p_bmp);

        /**
         * Convert parameter BMP file to a BMP file with only 128 colors
         * @param p_bmp BMP content to be converted
         * @return converted BMP content
         */
        inline
        lib_bmp::my_bmp
        compute_128_color_bmp(const lib_bmp::my_bmp & p_bmp);

        /**
         * Transform 128 color palette in 256 color palette
         * @param p_bmp BMP content whose palette should be extended
         */
        inline
        void
        extend_palette(lib_bmp::my_bmp & p_bmp);

        /**
         * Compute correspondancy between reference color and coding color
         * @param p_colors list of colors
         * @return correspondancy table
         */
        inline static
        std::map<lib_bmp::my_color, lib_bmp::my_color>
        compute_color_correspondance(const std::set<lib_bmp::my_color> & p_colors);

        /**
         * Compute color correspondance and apply GIF palette to BMP file
         * @param p_colors GIF color table
         * @param p_bmp BMP content
         * @return color correspondance
         */
        inline static
        std::map<lib_bmp::my_color, lib_bmp::my_color>
        compute_color_correspondance( const lib_gif::gif_color_table & p_colors
                                    , lib_bmp::my_bmp & p_bmp
                                    );

        /**
         * Encode data in a pixel information
         * @param p_x pixel coordinate on X axis
         * @param p_y pixel coordinate on Y axis
         * @param p_data bit to encode
         * @param p_swap indicate if swap should be apply between reference and coding color
         * @param p_color_correspondance color correspondance
         * @param p_bmp BMP content where data is encoded
         */
        inline
        void
        encode_pixel( unsigned int p_x
                    , unsigned int p_y
                    , bool p_data
                    , bool p_swap
                    , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                    , lib_bmp::my_bmp & p_bmp
                    );

        /**
         * Retrieve data encoded in pixel information
         * @param p_x pixel coordinate on X axis
         * @param p_y pixel coordinate on Y axis
         * @param p_swap indicate if swap should be apply between reference and coding color
         * @param p_color_correspondance color correspondance
         * @param p_bmp BMP content where data is encoded
         * @return encoded data
         */
        inline
        bool
        decode_pixel( unsigned int p_x
                    , unsigned int p_y
                    , bool p_swap
                    , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                    , lib_bmp::my_bmp & p_bmp
                    );

        /**
         * Generate list of pixels coordinate
         * @return list of pixels coordinate
         */
        inline static
        std::vector<std::pair<unsigned int, unsigned int>> generate_pixel_list(const lib_bmp::my_bmp & );


        inline
        void encode_picture( lib_bmp::my_bmp & p_bmp
                           , std::vector<uint8_t> & p_content
                           , std::vector<std::pair<unsigned int, unsigned int>> & p_pixels
                           , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                           , std::mt19937 & p_generator
                           , const uint64_t & p_offset
                           );

        inline
        void decode_picture( lib_bmp::my_bmp & p_bmp
                           , std::vector<uint8_t> & p_content
                           , std::vector<std::pair<unsigned int, unsigned int>> & p_pixels
                           , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                           , std::mt19937 & p_generator
                           );

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
         * Convert GIF color to BMP color
         * @param p_color GIF color
         * @return BMP color
         */
        inline static
        lib_bmp::my_color to_bmp_color(const lib_gif::gif_color & p_color);

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
    steganogif::encode( const std::string & p_output_file_name
                      , const std::string & p_content_file_name
                      , const std::string & p_transport_file_name
                      )
    {
        uint64_t l_content_size = 0;
#if __has_include(<filesystem>)
        l_content_size = std::filesystem::file_size(p_content_file_name);
#else // __has_include(<filesystem>)
        {
            std::ifstream l_content_file;
            l_content_file.open(p_content_file_name, std::ifstream::binary);
            if (l_content_file.is_open())
            {
                // get length of file:
                l_content_file.seekg(0, l_content_file.end);
                l_content_size = l_content_file.tellg();
                l_content_file.close();
            }
            else
            {
                throw quicky_exception::quicky_runtime_exception( R"(Unable to find file ")" + p_content_file_name + R"(")", __LINE__, __FILE__);
            }
        }
#endif // __has_include(<filesystem>)

        std::cout << "Content size : " << 8 * l_content_size << " bits" << std::endl;
        stegano_header l_header(l_content_size);
        std::vector<uint8_t> l_content{l_header.encode()};
        uint64_t l_header_size = l_content.size();
        l_content.resize(l_header_size + l_content_size + 20);
        std::cout << "Header + content size : " << 8 * l_content.size() << " bits" << std::endl;

        // BMP file
        lib_bmp::my_bmp l_bmp(p_transport_file_name);
        unsigned int l_bits_per_picture = l_bmp.get_height() * l_bmp.get_width();
        if(l_bits_per_picture % 8)
        {
            throw quicky_exception::quicky_logic_exception("Number of pixels in picture should be a multiple of 8", __LINE__, __FILE__);
        }

        unsigned int l_frame_number = 8 * l_content.size() / l_bits_per_picture;
        if(l_frame_number * l_bits_per_picture < 8 * l_content.size())
        {
            ++ l_frame_number;
        }
        std::cout << "Content size per picture : " << l_bits_per_picture << " bits" << std::endl;
        std::cout << "Number of picture : " << l_frame_number << std::endl;

        lib_bmp::my_bmp * l_work_bmp = &l_bmp;

        if(l_bmp.get_nb_bits_per_pixel() > 8)
        {
            std::cout << "Reduce number of colors" << std::endl;
            l_work_bmp = new lib_bmp::my_bmp(compute_128_color_bmp(l_bmp));

            extend_palette(*l_work_bmp);
            l_work_bmp->save("simplified.bmp");
        }

        // Check there are no duplicated colors in palette
        std::set<lib_bmp::my_color> l_colors;
        {
            std::map<lib_bmp::my_color, unsigned int> l_colors_index;
            for (unsigned int l_index = 0; l_index < l_work_bmp->get_palette().get_size(); ++l_index)
            {
                //std::cout << l_bmp_256.get_palette().get_color(l_index) << std::endl;
                if (l_colors_index.count(l_work_bmp->get_palette().get_color(l_index)))
                {
                    std::stringstream l_color_stream;
                    l_color_stream << l_work_bmp->get_palette().get_color(l_index);
                    throw quicky_exception::quicky_logic_exception( "Color duplicated at index " + std::to_string(l_index) + " / " + std::to_string(l_colors_index.find(l_work_bmp->get_palette().get_color(l_index))->second) + " : " + l_color_stream.str(), __LINE__, __FILE__);
                }
                l_colors_index.insert(std::make_pair(l_work_bmp->get_palette().get_color(l_index), l_index));
                l_colors.insert(l_work_bmp->get_palette().get_color(l_index));
            }
        }

        std::cout << "Compute color correspondances " << std::endl;
        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance = compute_color_correspondance(l_colors);

        // Read file content
        {
            std::cout << "Read content to hide" << std::endl;
            std::ifstream l_content_file;
            l_content_file.open(p_content_file_name, std::ifstream::binary);
            if (l_content_file.is_open())
            {
                l_content_file.read((char*)l_content.data() + l_header_size, l_content_size);
                l_content_file.close();
                std::cout << "Compute hash" << std::endl;
                sha1 l_content_sha1(l_content.data() + l_header_size, l_content_size);
                for(unsigned int l_index = 0; l_index < 5; ++l_index)
                {
                    auto * l_ptr = (uint32_t*)&l_content[l_content.size() - 5 * sizeof(uint32_t)+ l_index * sizeof(uint32_t)];
                    * l_ptr = l_content_sha1.get_key(l_index);
                }
            } else
            {
                throw quicky_exception::quicky_runtime_exception( R"(Unable to find file ")" + p_content_file_name + R"(")", __LINE__, __FILE__);
            }
        }

        std::vector<std::pair<unsigned int, unsigned int>> l_pixels = generate_pixel_list(*l_work_bmp);
        uint64_t l_offset = 0;
        std::mt19937 l_generator{*m_seed};

        std::ofstream l_output_gif;
        l_output_gif.open(p_output_file_name, std::ofstream::binary);
        if(!l_output_gif.is_open())
        {
            throw quicky_exception::quicky_runtime_exception(R"(Unable to create file ")" + p_output_file_name + R"(")", __LINE__, __FILE__);
        }

        gif_streamer::gif_streamer l_gif_streamer{l_output_gif, l_work_bmp->get_width(), l_work_bmp->get_height()};
        for(unsigned int l_frame_index = 0; l_frame_index < l_frame_number; ++l_frame_index)
        {
            std::cout << "Encode picture " << l_frame_index << std::endl;
            encode_picture(*l_work_bmp, l_content, l_pixels, l_color_correspondance, l_generator, l_offset);
            std::string l_bmp_name = std::to_string(l_frame_index) + ".bmp";
            l_work_bmp->save(l_bmp_name);
            l_gif_streamer.send_bmp(l_bmp_name);
            l_offset += l_bits_per_picture / 8;
        }

        l_gif_streamer.send_trailer();
        l_output_gif.close();

        if(l_work_bmp != &l_bmp)
        {
            delete l_work_bmp;
        }
    }

    //-------------------------------------------------------------------------
    void
    steganogif::decode( const std::string & p_input_file_name
                      , const std::string & p_content_file_name
                      )
    {
        std::ifstream l_gif_file;
        l_gif_file.open(p_input_file_name.c_str(), std::ifstream::binary);
        if(!l_gif_file.is_open())
        {
            throw quicky_exception::quicky_runtime_exception(R"(Unable to read file ")" + p_input_file_name + R"(")", __LINE__, __FILE__);
        }

        lib_gif::gif l_gif(l_gif_file);

        l_gif_file.close();

        unsigned int l_bits_per_picture = l_gif.get_height() * l_gif.get_width();
        if(l_bits_per_picture % 8)
        {
            throw quicky_exception::quicky_logic_exception("Number of pixels in picture should be a multiple of 8", __LINE__, __FILE__);
        }
        std::cout << "Content size per picture : " << l_bits_per_picture << " bits" << std::endl;

        lib_bmp::my_bmp l_bmp(l_gif.get_width(), l_gif.get_height(), 8);

        // Get global palette from GIF
        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance;
        lib_gif::gif_color_table const * l_color_table = nullptr;
        if(l_gif.get_global_color_table_flag())
        {
            l_color_table = & l_gif.get_global_color_table();

            l_color_correspondance = compute_color_correspondance(*l_color_table, l_bmp);

            // Set background
            lib_gif::gif_color l_color = (*l_color_table)[l_gif.get_background_index()];
            lib_bmp::my_color l_bmp_color = to_bmp_color(l_color);
            for(unsigned int l_y = 0 ; l_y < l_gif.get_height() ; ++l_y)
            {
                for(unsigned int l_x = 0 ; l_x < l_gif.get_width() ; ++l_x)
                {
                    l_bmp.set_pixel_color(l_x,l_y,lib_bmp::my_color_alpha(l_bmp_color));
                }
            }

        }

        unsigned int l_frame_index = 0;
        std::vector<uint8_t> l_content;
        std::mt19937 l_generator{*m_seed};
        std::vector<std::pair<unsigned int, unsigned int>> l_pixels = generate_pixel_list(l_bmp);
        unsigned int l_content_size = 0;

        const lib_gif::gif_graphic_control_extension * l_control_extension = nullptr;
        for(unsigned int l_index = 0 ; l_index < l_gif.get_nb_data_block(); ++l_index)
        {
            const lib_gif::gif_data_block & l_data_block = l_gif.get_data_block(l_index);
            switch (l_data_block.get_type())
            {
                case lib_gif::gif_data_block::t_gif_data_block_type::GRAPHICAL_CONTROL_EXTENSION:
                    l_control_extension = dynamic_cast<const lib_gif::gif_graphic_control_extension*>(&l_data_block);
                    break;
                case lib_gif::gif_data_block::t_gif_data_block_type::COMMENT_EXTENSION:
                case lib_gif::gif_data_block::t_gif_data_block_type::APPLICATION_EXTENSION:
                    break;
                case lib_gif::gif_data_block::t_gif_data_block_type::GRAPHIC_BLOCK:
                {
                    const lib_gif::gif_graphic_block & l_graphic_block = * dynamic_cast<const lib_gif::gif_graphic_block*>(&l_data_block);
                    const unsigned int l_left_position = l_graphic_block.get_left_position();
                    const unsigned int l_top_position = l_graphic_block.get_top_position();
                    const unsigned int l_width = l_graphic_block.get_width();
                    const unsigned int l_height = l_graphic_block.get_height();

                    std::string l_error;
                    if(l_width + l_left_position > l_gif.get_width())
                    {
                        l_error = "Max x coordinate " + std::to_string(l_width + l_left_position) + " is greater than width " + std::to_string(l_gif.get_width());
                    }
                    if(l_height + l_top_position > l_gif.get_height())
                    {
                        l_error = "Max y coordinate " + std::to_string(l_height + l_top_position) + " is greater than height " + std::to_string(l_gif.get_height());
                    }
                    if(!l_error.empty())
                    {
                        throw quicky_exception::quicky_logic_exception(l_error,__LINE__,__FILE__);
                    }

                    lib_bmp::my_bmp * l_saved_rectangle = nullptr;
                    if(l_control_extension && 3 == l_control_extension->get_disposal_method())
                    {
                        l_saved_rectangle = new lib_bmp::my_bmp(l_width, l_height, 8);
                        for(unsigned int l_y = 0; l_y < l_height; ++l_y)
                        {
                            for(unsigned int l_x = 0; l_x < l_width; ++l_x)
                            {
                                l_saved_rectangle->set_pixel_color(l_x, l_y, l_bmp.get_pixel_color(l_x + l_left_position, l_y + l_top_position));
                            }
                        }
                    }

                    if(l_graphic_block.is_image())
                    {
                        const lib_gif::gif_image & l_image = l_graphic_block.get_image();
                        lib_gif::gif_color_table const * l_saved_color_table = l_color_table;
                        if(l_image.get_local_color_table_flag())
                        {
                            l_color_table = & l_image.get_local_color_table();
                            l_color_correspondance = compute_color_correspondance(*l_color_table, l_bmp);
                        }
                        if(!l_color_table)
                        {
                            throw quicky_exception::quicky_logic_exception("No colour table available",__LINE__,__FILE__);
                        }

                        for(unsigned int l_y = 0 ; l_y < l_height ; ++l_y)
                        {
                            unsigned int l_computed_y = !l_image.get_interlace_flag() ? l_y : l_image.deinterlace(l_y);
                            for(unsigned int l_x = 0 ; l_x < l_width ; ++l_x)
                            {
                                if(!l_control_extension || !l_control_extension->get_transparent_color_flag() || l_control_extension->get_transparent_color_index() != l_image.get_color_index(l_x,l_y))
                                {
                                    lib_gif::gif_color l_color = (*l_color_table)[l_image.get_color_index(l_x,l_computed_y)];
                                    lib_bmp::my_color l_bmp_color = to_bmp_color(l_color);
                                    l_bmp.set_pixel_color(l_left_position + l_x,l_top_position + l_y,lib_bmp::my_color_alpha(l_bmp_color));
                                }
                            }
                        }
                        std::cout << "Decode picture " << std::to_string(l_frame_index) << std::endl;
                        decode_picture(l_bmp, l_content, l_pixels, l_color_correspondance, l_generator);

                        if(!l_frame_index)
                        {
                            stegano_header l_header{l_content};
                            l_content_size = l_header.get_size();
                            std::cout << "Content size : " << 8 * l_content_size << " bits" << std::endl;
                        }
                        l_bmp.save("decoded_" + std::to_string(l_frame_index) + ".bmp");
                        ++l_frame_index;
                        l_color_table = l_saved_color_table;
                    }
                    if(l_control_extension)
                    {
                        switch(l_control_extension->get_disposal_method())
                        {
                            case 0:
                            case 1:
                                break;
                            case 2:
                                if(l_gif.get_global_color_table_flag())
                                {
                                    l_color_table = & l_gif.get_global_color_table();
                                    lib_gif::gif_color l_color = (*l_color_table)[l_gif.get_background_index()];
                                    lib_bmp::my_color l_bmp_color = to_bmp_color(l_color);
                                    for(unsigned int l_y = 0; l_y < l_height; ++l_y)
                                    {
                                        for(unsigned int l_x = 0; l_x < l_width; ++l_x)
                                        {
                                            l_bmp.set_pixel_color(l_x + l_left_position, l_y + l_top_position, lib_bmp::my_color_alpha(l_bmp_color));
                                        }
                                    }
                                }
                                break;
                            case 3:
                            {
                                for(unsigned int l_y = 0; l_y < l_height; ++l_y)
                                {
                                    for(unsigned int l_x = 0; l_x < l_width; ++l_x)
                                    {
                                        l_bmp.set_pixel_color(l_x + l_left_position, l_y + l_top_position, l_saved_rectangle->get_pixel_color(l_x, l_y));
                                    }
                                }
                                delete l_saved_rectangle;
                                l_saved_rectangle = nullptr;
                            }
                                break;
                            default:
                                std::cout << "Unsupported disposal method : " << l_control_extension->get_disposal_method() << std::endl ;
                        }
                    }
                    l_control_extension = nullptr;
                }
                    break;
                case lib_gif::gif_data_block::t_gif_data_block_type::TRAILER:
                    break;
                default:
                    throw quicky_exception::quicky_logic_exception("Unsupported gif_data_block type "+lib_gif::gif_data_block::type_to_string(l_data_block.get_type()),__LINE__,__FILE__);

            }
        }

        try
        {
            // To have number of frame
            ++l_frame_index;
            if(l_frame_index * l_bits_per_picture < 8 * l_content.size())
            {
                unsigned int l_expected_frame_number = 8 * l_content_size / l_bits_per_picture + !!((8 * l_content_size) % l_bits_per_picture);
                throw quicky_exception::quicky_logic_exception("Insufficant number of frame (" + std::to_string(l_frame_index) + " regarding number required ("+ std::to_string(l_expected_frame_number) +") according to declared content size", __LINE__, __FILE__);
            }
        }
        catch(std::exception & e)
        {
            std::cout << "No content associated with this password" << std::endl;
            return;
        }

        // Check if content was retrieved
        if(l_content.size() == l_content_size + 20)
        {
            std::cout << "No content associated with this password" << std::endl;
            return;
        }
        // Check SHA1
        sha1 l_sha1(l_content.data(), l_content_size);
        for(unsigned int l_index = 0; l_index < 5; ++ l_index)
        {
            auto l_ptr = (uint32_t*)(l_content.data() + l_content_size + l_index * sizeof(uint32_t));
            if(l_sha1.get_key(l_index) != *l_ptr)
            {
                std::cout << "No content associated with this password" << std::endl;
                return;
            }
        }

        std::ofstream l_content_file;
        l_content_file.open(p_content_file_name, std::ofstream::binary);
        if(!l_content_file.is_open())
        {
            throw quicky_exception::quicky_runtime_exception(R"(Unable to create file ")" + p_content_file_name + R"(")", __LINE__, __FILE__);
        }
        l_content_file.write((char*)l_content.data(), l_content_size);
        l_content_file.close();
        std::cout << R"(Content extracted in ")" << p_content_file_name << R"(")" << std::endl;
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

    //-------------------------------------------------------------------------
    lib_bmp::my_bmp
    steganogif::compute_128_color_bmp(const lib_bmp::my_bmp & p_bmp)
    {
        {
            // Copy BMP content
            lib_bmp::my_bmp l_new_bmp(p_bmp.get_width(), p_bmp.get_height(), 8);
            for (unsigned int l_y = 0; l_y < p_bmp.get_height(); ++l_y)
            {
                for (unsigned int l_x = 0; l_x < p_bmp.get_width(); ++l_x)
                {
                    l_new_bmp.set_pixel_color(l_x, l_y, p_bmp.get_pixel_color(l_x, l_y));
                }
            }

            // Create 128 color palette
            std::array<uint8_t, 4> l_red_components
            { 0
            , 64
            , 128
            , 255
            };

            std::array<uint8_t, 8> l_green_components
            { 0
            , 32
            , 64
            , 96
            , 128
            , 160
            , 192
            , 255
            };

            unsigned int l_index = 0;
            for (auto l_iter_red: l_red_components)
            {
                for (auto l_iter_green: l_green_components)
                {
                    for (auto l_iter_blue: l_red_components)
                    {
                        l_new_bmp.get_palette().set_color(lib_bmp::my_color_alpha(l_iter_red, l_iter_green, l_iter_blue), l_index);
                        ++l_index;
                    }
                }
            }
            assert(128 == l_index);
            while (l_index < 256)
            {
                l_new_bmp.get_palette().set_color(lib_bmp::my_color_alpha(0,0,0), l_index);
                ++l_index;
            }

            l_new_bmp.save("128_color.bmp");
        }
        lib_bmp::my_bmp l_new_bmp{"128_color.bmp"};
        return l_new_bmp;
    }

    //-------------------------------------------------------------------------
    void
    steganogif::extend_palette(lib_bmp::my_bmp & p_bmp)
    {
        // obtain a seed from the system clock:
        std::mt19937 l_color_generator{(unsigned int)std::chrono::system_clock::now().time_since_epoch().count()};
        for(unsigned int l_index = 128;l_index < 256; ++l_index)
        {
            unsigned int l_componant_index = l_color_generator() % 3;
            assert(lib_bmp::my_color_alpha(0,0,0) == p_bmp.get_palette().get_color(l_index));
            lib_bmp::my_color_alpha l_original_color = p_bmp.get_palette().get_color(l_index - 128);
#ifdef VERBOSE_STEGANOGIF
            std::cout << "[" << l_index << "] " << l_original_color << " => " ;
#endif // VERBOSE_STEGANOGIF
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
#ifdef VERBOSE_STEGANOGIF
            std::cout << l_original_color << std::endl ;
#endif // VERBOSE_STEGANOGIF
            p_bmp.get_palette().set_color(l_original_color, l_index);
        }
    }

    //-------------------------------------------------------------------------
    std::map<lib_bmp::my_color, lib_bmp::my_color>
    steganogif::compute_color_correspondance(const std::set<lib_bmp::my_color> & p_colors)
    {
        if(p_colors.size() % 2)
        {
            throw quicky_exception::quicky_logic_exception("Number of color should be even : " + std::to_string(p_colors.size()), __LINE__, __FILE__);
        }
        std::set<lib_bmp::my_color> l_colors{p_colors};
        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance;
        while(l_colors.size())
        {
#ifdef VERBOSE_STEGANOGIF
            std::cout << l_colors.size() << std::endl;
#endif // VERBOSE_STEGANOGIF
            float l_min = std::numeric_limits<float>::max();
            lib_bmp::my_color l_lower_color;
            lib_bmp::my_color l_upper_color;
            for(auto l_iter: l_colors)
            {
                auto l_search_min = [=, &l_min, &l_lower_color, &l_upper_color](const auto p_color)
                {
                    if (p_color != l_iter)
                    {
                        float l_dist = dist(p_color, l_iter);
                        if (l_dist < l_min)
                        {
                            l_min = l_dist;
                            l_lower_color = p_color;
                            l_upper_color = l_iter;
                        }
                    }
                };
                std::for_each(l_colors.begin(), l_colors.end(), l_search_min);
            }
#ifdef VERBOSE_STEGANOGIF
            std::cout << l_upper_color << " <==> " << l_lower_color << " : " << l_min << std::endl;
#endif // VERBOSE_STEGANOGIF
            l_color_correspondance.insert(std::make_pair(l_lower_color, l_upper_color));
            l_color_correspondance.insert(std::make_pair(l_upper_color, l_lower_color));
            l_colors.erase(l_upper_color);
            l_colors.erase(l_lower_color);
        }
        return l_color_correspondance;
    }

    //-------------------------------------------------------------------------
    void
    steganogif::encode_pixel( unsigned int p_x
                            , unsigned int p_y
                            , bool p_data
                            , bool p_swap
                            , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                            , lib_bmp::my_bmp & p_bmp
                            )
    {
        lib_bmp::my_color l_origin_color = p_bmp.get_pixel_color(p_x, p_y);
        auto l_iter = p_color_correspondance.find(l_origin_color);
        assert(l_iter != p_color_correspondance.end());
        lib_bmp::my_color l_related_color = l_iter->second;
        if(l_origin_color > l_related_color)
        {
            std::swap(l_origin_color, l_related_color);
        }
        if(p_data ^ p_swap)
        {
            p_bmp.set_pixel_color(p_x, p_y, lib_bmp::my_color_alpha(l_related_color));
        }
        else
        {
            p_bmp.set_pixel_color(p_x, p_y, lib_bmp::my_color_alpha(l_origin_color));
        }
    }

    //-------------------------------------------------------------------------
    bool
    steganogif::decode_pixel( unsigned int p_x
                            , unsigned int p_y
                            , bool p_swap
                            , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                            , lib_bmp::my_bmp & p_bmp
                            )
    {
        const lib_bmp::my_color & l_pixel_color = p_bmp.get_pixel_color(p_x, p_y);
        auto l_iter = p_color_correspondance.find(l_pixel_color);
        assert(l_iter != p_color_correspondance.end());
        lib_bmp::my_color l_related_color = l_iter->second;
        bool l_data = l_related_color < l_pixel_color;
        l_data = l_data ^ p_swap;
        return l_data;
    }

    //-------------------------------------------------------------------------
    std::vector<std::pair<unsigned int, unsigned int>>
    steganogif::generate_pixel_list(const lib_bmp::my_bmp & p_bmp)
    {
        std::vector<std::pair<unsigned int, unsigned int>> l_pixels;
        for(unsigned int l_y = 0; l_y < p_bmp.get_height(); ++l_y)
        {
            for(unsigned int l_x = 0; l_x < p_bmp.get_width(); ++l_x)
            {
                l_pixels.emplace_back(std::make_pair(l_x, l_y));
            }
        }
        return l_pixels;
    }

    //-------------------------------------------------------------------------
    void
    steganogif::encode_picture( lib_bmp::my_bmp & p_bmp
                              , std::vector<uint8_t> & p_content
                              , std::vector<std::pair<unsigned int, unsigned int>> & p_pixels
                              , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
                              , std::mt19937 & p_generator
                              , const uint64_t & p_offset
                              )
    {
        unsigned int l_remaining_pixel_index = p_bmp.get_width() * p_bmp.get_height();
        std::mt19937 l_data_generator{(unsigned int)std::chrono::system_clock::now().time_since_epoch().count()};
        if(!l_remaining_pixel_index)
        {
            throw quicky_exception::quicky_logic_exception("Pixel index start at zero", __LINE__, __FILE__);
        }
        for(unsigned int l_pixel_index = 0; l_remaining_pixel_index; ++l_pixel_index, --l_remaining_pixel_index)
        {
            unsigned int l_swap_index = l_pixel_index + (p_generator() % l_remaining_pixel_index);
            std::swap(p_pixels[l_pixel_index], p_pixels[l_swap_index]);
            unsigned int l_byte_index = p_offset + l_pixel_index / 8;
            bool l_data;
            if(l_byte_index < p_content.size())
            {
                unsigned int l_bit_index = l_pixel_index % 8;
                l_data = (p_content[l_byte_index] & (1u << l_bit_index));
            }
            else
            {
                l_data = l_data_generator() % 2;
            }
            bool l_swap = p_generator() % 2;
            encode_pixel(p_pixels[l_pixel_index].first, p_pixels[l_pixel_index].second, l_data, l_swap, p_color_correspondance, p_bmp);
        }
    }

    //-------------------------------------------------------------------------
    void
    steganogif::decode_picture( lib_bmp::my_bmp & p_bmp
            , std::vector<uint8_t> & p_content
            , std::vector<std::pair<unsigned int, unsigned int>> & p_pixels
            , const std::map<lib_bmp::my_color, lib_bmp::my_color> & p_color_correspondance
            , std::mt19937 & p_generator
                              )
    {
        unsigned int l_remaining_pixel_index = p_bmp.get_width() * p_bmp.get_height();
        if(!l_remaining_pixel_index)
        {
            throw quicky_exception::quicky_logic_exception("Pixel index start at zero", __LINE__, __FILE__);
        }
        uint8_t l_byte = 0;
        for(unsigned int l_pixel_index = 0; l_remaining_pixel_index; ++l_pixel_index, --l_remaining_pixel_index)
        {
            unsigned int l_swap_index = l_pixel_index + (p_generator() % l_remaining_pixel_index);
            std::swap(p_pixels[l_pixel_index], p_pixels[l_swap_index]);
            unsigned int l_bit_index = l_pixel_index % 8;
            bool l_swap = p_generator() % 2;
            bool l_data = decode_pixel(p_pixels[l_pixel_index].first, p_pixels[l_pixel_index].second, l_swap, p_color_correspondance, p_bmp);
            l_byte |= ((unsigned int) l_data) << l_bit_index;
            if(7 == l_bit_index)
            {
                p_content.emplace_back(l_byte);
                l_byte = 0;
            }
        }
    }

    //-------------------------------------------------------------------------
    lib_bmp::my_color
    steganogif::to_bmp_color(const lib_gif::gif_color & p_color)
    {
        return lib_bmp::my_color(p_color.get_red(), p_color.get_green(), p_color.get_blue());
    }

    //-------------------------------------------------------------------------
    std::map<lib_bmp::my_color, lib_bmp::my_color>
    steganogif::compute_color_correspondance( const lib_gif::gif_color_table & p_color_table
                                            , lib_bmp::my_bmp & p_bmp
                                            )
    {
        std::map<lib_bmp::my_color, lib_bmp::my_color> l_color_correspondance;
        std::set<lib_bmp::my_color> l_colors;
        for(unsigned int l_color_index = 0; l_color_index < p_color_table.get_size(); ++l_color_index)
        {
            lib_gif::gif_color l_gif_color = p_color_table[l_color_index];
            lib_bmp::my_color l_bmp_color = to_bmp_color(l_gif_color);
            p_bmp.get_palette().set_color(lib_bmp::my_color_alpha(l_bmp_color), l_color_index);
            l_colors.insert(l_bmp_color);
        }
        for(unsigned int l_color_index = p_color_table.get_size(); l_color_index < 256; ++l_color_index)
        {
            lib_bmp::my_color l_bmp_color;
            p_bmp.get_palette().set_color(lib_bmp::my_color_alpha(l_bmp_color), l_color_index);
            l_colors.insert(l_bmp_color);
        }
        if(l_colors.size() % 2)
        {
            throw quicky_exception::quicky_logic_exception("Number of color is not odd", __LINE__, __FILE__);
        }
        l_color_correspondance = compute_color_correspondance(l_colors);

        return l_color_correspondance;
    }

}
#endif // STEGANOGIF_H
// EOF