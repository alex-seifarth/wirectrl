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
#include <core/ini.h>

#include <algorithm>
#include <regex>
#include <sstream>

// taken from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
void core::ini::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// taken from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
void core::ini::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// taken from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
void core::ini::trim(std::string &s) {
    core::ini::ltrim(s);
    core::ini::rtrim(s);
}

void core::ini::trim_quotes(std::string& s)
{
    if (s.size() > 1 && (
            (s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''))) {
        s = s.substr(1, s.size()-2);
    }
}

std::pair<std::string, int> core::ini::read_line_ext(std::istream& input)
{
    std::stringstream ss;
    int line_count{0};
    bool extend;
    if (input.eof()) {
        return std::make_pair(std::string{}, 0);
    }
    do {
        std::string line;
        std::getline(input, line);
        if (input.bad() || (input.fail() && !input.eof())) {
            throw std::runtime_error{"input stream bad"};
        }
        core::ini::trim(line);
        if (!input.eof() || !line.empty()) {
            ++line_count;
        }
        if (!line.empty() && line.back() == '\\') {
            line = line.substr(0, line.size()-1);
            if (input.eof()) {
                throw std::runtime_error{"unexpected end-of-file"};
            }
            extend = true;
        }
        else {
            extend = false;
        }
        ss << line;
    } while(extend);
    return std::make_pair(ss.str(), line_count);
};

// ----------------------------------------------------------------------------
// file
// ----------------------------------------------------------------------------

namespace {

    core::ini::section process_section_header(std::string const& line, int line_number, bool remove_quotes)
    {
        static const std::regex section_regex{R"(\[([A-Za-z0-9_\-:]+)(\s*=\s*(.*))?\])"};

        core::ini::section section{};
        section.line_number = line_number;
        std::smatch base_match;
        if (!std::regex_match(line, base_match, section_regex) || base_match.size() < 2) {
            throw core::ini::parse_exception{line_number, line};
        }
        section.name = base_match[1];
        if (base_match.size() > 3) {
            section.value = base_match[3];
            if (remove_quotes) {
                core::ini::trim_quotes(section.value);
            }
        }
        return section;
    }

    core::ini::property process_property(std::string const& line, int line_number, bool remove_quotes)
    {
        static const std::regex property_regex{R"(\s*([a-zA-Z0-9_\.\-:]+)\s*=\s*(.+)?)"};

        core::ini::property property{};
        property.line_number = line_number;
        std::smatch base_match;
        if (!std::regex_match(line, base_match, property_regex) || base_match.size() < 3) {
            throw core::ini::parse_exception{line_number, line};
        }
        property.name = base_match[1];
        if (base_match.size() > 2) {
            property.value = base_match[2];
        }
        if (remove_quotes) {
            core::ini::trim_quotes(property.value);
        }
        return property;
    }

} // namespace

core::ini::file::file() = default;

core::ini::file::~file() = default;

core::ini::file::file(std::istream& input, bool remove_value_quotes)
    : core::ini::file::file{}
{
    if (input.fail() || input.bad()) {
        throw std::runtime_error{"input file not readable"};
    }

    int line_number{1};
    int lc;
    std::string line;
    core::ini::section* current_section{nullptr};
    while(input.good()) {
        std::tie(line, lc) = read_line_ext(input);
        if (line.empty() || line.front() == '#') {
            continue;
        }
        else if (line.front() == '[') {
            _sections.push_back(process_section_header(line, line_number, remove_value_quotes));
            current_section = &_sections.back();
        }
        else {
            if (!current_section) {
                _sections.push_back(core::ini::section{}); // section with empty name == "root" section
                current_section = &_sections.back();
            }
            current_section->properties.push_back(process_property(line, line_number, remove_value_quotes));
        }
        line_number += lc;
    }
}

std::vector<core::ini::section> const& core::ini::file::sections() const
{
    return _sections;
}

// ----------------------------------------------------------------------------
// parse_exception
// ----------------------------------------------------------------------------
core::ini::parse_exception::parse_exception(int line_number, std::string expr)
    : _expression{std::move(expr)}, _line_number{line_number}
{}

core::ini::parse_exception::~parse_exception() = default;

const char* core::ini::parse_exception::what() const noexcept
{
    return "syntax error while parsing";
}

std::string const& core::ini::parse_exception::expression() const noexcept
{
    return _expression;
}

int core::ini::parse_exception::line_number() const noexcept
{
    return _line_number;
}
