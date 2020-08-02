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
#include <iniread/iniread.h>

#include <algorithm>
#include <cassert>
#include <regex>
#include <stdexcept>

using namespace iniread;

namespace {
    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    std::string read_line(std::istream& input, int& line_number)
    {
        std::stringstream ss;
        bool cont;
        do {
            if (input.bad() || input.fail()) {
                throw std::runtime_error{std::string{"error reading line "} + std::to_string(line_number)} ;
            }
            if (input.eof()) {
                throw std::runtime_error{"unexpected end of file"};
            }
            std::string line;
            std::getline(input, line);
            ++line_number;
            trim(line);
            if (!line.empty() && line.back() == '\\') {
                line = line.substr(0, line.size()-2);
                trim(line);
                cont = true;
            }
            else {
                cont = false;
            }
            ss << line;
        } while(cont);
        return ss.str();
    }


    iniread::section process_section_header(std::string const& line, int line_number)
    {
        static const std::regex section_regex{R"(\[([A-Za-z0-9_\-:]+)(=(.*))?\])"};

        iniread::section section{};
        std::smatch base_match;
        if (!std::regex_match(line, base_match, section_regex) || base_match.size() < 2) {
            throw std::runtime_error{std::string{"invalid section header syntax line:"} + std::to_string(line_number)};
        }
        section.line_number = line_number;
        section.name = base_match[1];
        if (base_match.size() > 2) {
            section.value = base_match[2];
        }
        return section;
    }

    iniread::property process_property(std::string const& line, int line_number)
    {
        static const std::regex property_regex{R"(\s*([a-zA-Z0-9_\.\-:]+)\s*=\s*(.+))"};

        iniread::property property{};
        std::smatch base_match;
        if (!std::regex_match(line, base_match, property_regex) || base_match.size() < 3) {
            throw std::runtime_error{std::string{"invalid property syntax line:"} + std::to_string(line_number)};
        }
        property.name = base_match[1];
        property.line_number = line_number;
        property.value = base_match[2];
        if (property.value.size() > 1 && property.value.front() == '"' && property.value.back() == '"') {
            property.value = property.value.substr(1, property.value.size()-2);
        }
        return property;
    }

} // namespace


section iniread::read_ini_file(std::istream& input)
{
    if (input.fail() || input.bad()) {
        throw std::runtime_error{"input file not readable"};
    }

    iniread::section root_section{};
    section* current_section{&root_section};
    int line_number{0};
    while(input.good()) {
        std::string line = read_line(input, line_number);
        if (line.empty() || line.front() == '#') {
            continue;
        }
        else if (line.front() == '[') {
            root_section.subsections.push_back(process_section_header(line, line_number));
            current_section = &root_section.subsections.back();
        }
        else {
            assert(current_section);
            current_section->properties.push_back(process_property(line, line_number));
        }
    }
    return root_section;
}
