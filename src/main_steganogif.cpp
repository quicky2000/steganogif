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

int main()
{
    // Get password
    std::string l_password;
    getpass2(l_password, "Password ?");
    std::cout << R"(You enter password ")" << l_password << R"(")" << std::endl;

    steganogif::steganogif l_steganogif{l_password};

    l_steganogif.encode();

    return 0;
}
// EOF