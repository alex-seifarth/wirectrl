// wirectrl is a daemon for systemd to control GPIO ports of raspberry pi
// Copyright (C) 2020 Alexander Seifarth
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <doctest/doctest.h>
#include <core/ini.h>

TEST_CASE("ltrim")
{
    std::string s1{"  ajfk  "};
    core::ini::ltrim(s1);
    CHECK_EQ(s1, std::string{"ajfk  "});

    std::string s3{" \t  ajfk  "};
    core::ini::ltrim(s3);
    CHECK_EQ(s3, std::string{"ajfk  "});

    std::string s5{"jf\t k"};
    core::ini::ltrim(s5);
    CHECK_EQ(s5, std::string{"jf\t k"});
}

TEST_CASE("rtrim")
{
    std::string s2{"  ajfk   "};
    core::ini::rtrim(s2);
    CHECK_EQ(s2, std::string{"  ajfk"});

    std::string s4{" \t  ajfk  \t\t  "};
    core::ini::rtrim(s4);
    CHECK_EQ(s4, std::string{" \t  ajfk"});

    std::string s6{"jf\t k"};
    core::ini::rtrim(s6);
    CHECK_EQ(s6, std::string{"jf\t k"});
}

TEST_CASE("trim")
{
    std::string s2{"  ajfk   "};
    core::ini::trim(s2);
    CHECK_EQ(s2, std::string{"ajfk"});

    std::string s4{" \t  ajfk  \t\t  "};
    core::ini::trim(s4);
    CHECK_EQ(s4, std::string{"ajfk"});

    std::string s6{"jf\t k"};
    core::ini::trim(s6);
    CHECK_EQ(s6, std::string{"jf\t k"});

    std::string s8{"jf\t k "};
    core::ini::trim(s8);
    CHECK_EQ(s8, std::string{"jf\t k"});
}


