/*
      This file is part of duplication_checker
      Copyright (C) 2020 Julien Thevenon ( julien_thevenon at yahoo.fr )

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

#include "password_input.h"
#include "steganogif.h"
#include "parameter_manager.h"

int main(int p_argc, char ** p_argv)
{
    try
    {
        // Defining application command line parameters
        parameter_manager::parameter_manager l_param_manager("steganogif.exe","--",2);
        parameter_manager::parameter_if l_gif_file_name_parameter("gif", false);
        l_param_manager.add(l_gif_file_name_parameter);
        parameter_manager::parameter_if l_content_file_name_parameter("content", false);
        l_param_manager.add(l_content_file_name_parameter);
        parameter_manager::parameter_if l_bmp_file_name_parameter("bmp", true);
        l_param_manager.add(l_bmp_file_name_parameter);
        parameter_manager::parameter_if l_password_parameter("password", true);
        l_param_manager.add(l_password_parameter);

        // Treating parameters
        l_param_manager.treat_parameters(p_argc, p_argv);

        // Get password
        auto l_password = l_password_parameter.get_value<std::string>();
        if(l_password.empty())
        {
            getpass2(l_password, "Password ?");
            std::cout << R"(You enter password ")" << l_password << R"(")" << std::endl;
        }

        steganogif::steganogif l_steganogif{l_password};

        auto l_gif_file_name = l_gif_file_name_parameter.get_value<std::string>();
        auto l_bmp_file_name = l_bmp_file_name_parameter.get_value<std::string>();
        auto l_content_file_name = l_content_file_name_parameter.get_value<std::string>();

        if(l_bmp_file_name.empty())
        {
            l_steganogif.decode(l_gif_file_name, l_content_file_name);
        }
        else
        {
            l_steganogif.encode(l_gif_file_name, l_content_file_name, l_bmp_file_name);
        }

    }
   catch(quicky_exception::quicky_runtime_exception & e)
    {
        std::cout << "ERROR : " << e.what() << " from " << e.get_file() << ":" << e.get_line() << std::endl ;
        return(-1);
    }
    catch(quicky_exception::quicky_logic_exception & e)
    {
        std::cout << "ERROR : " << e.what() << " from " << e.get_file() << ":" << e.get_line() << std::endl ;
        return(-1);
    }

    return 0;
}
// EOF