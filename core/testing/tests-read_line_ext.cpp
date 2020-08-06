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
#include <sstream>

TEST_CASE("read_line_ext")
{
    SUBCASE("single standard line without new line"){
        std::string text{"this is a text line"};
        std::istringstream istr{text};

        auto v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, text);
        CHECK_EQ(v.second, 1);

        v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, "");
        CHECK_EQ(v.second, 0);
    }

    SUBCASE("single standard line with new line"){
        std::string text{"this is a text line\n"};
        std::istringstream istr{text};

        auto v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, "this is a text line");
        CHECK_EQ(v.second, 1);

        v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, "");
        CHECK_EQ(v.second, 0);
    }

    SUBCASE("single extended line"){
        std::string text{"this is a text line \\ \n alpha beta"};
        std::istringstream istr{text};

        auto v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, "this is a text line alpha beta");
        CHECK_EQ(v.second, 2);

        v = core::ini::read_line_ext(istr);
        CHECK_EQ(v.first, "");
        CHECK_EQ(v.second, 0);
    }

    SUBCASE("empty stream") {
        std::string text{};
        std::istringstream istr{text};

        auto v = core::ini::read_line_ext(istr);
        CHECK(v.first.empty());
        CHECK_EQ(v.second, 0);
    }
}
