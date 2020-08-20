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
#pragma once

#include <exception>
#include <istream>
#include <string>
#include <vector>

namespace core::ini {

    //! @brief Trims whitespaces from left from the string.
    //! @see std::isspace
    //! @todo Should go into another namespace, not ini
    void ltrim(std::string &s);

    //! @brief Trims whitespaces from right from the string
    //! @see std::isspace
    //! @todo Should go into another namespace, not ini
    void rtrim(std::string &s);

    //! @brief Trims whitespaces from left and right from the string
    //! @see std::isspace
    //! @todo Should go into another namespace, not ini
    void trim(std::string &s);

    //! @brief If the string is begins/ends with quotes " or apostrophe ' the method
    //!     removes these.
    void trim_quotes(std::string& s);

    //! @brief Reads a line from the input stream and trims white spaces from left and right
    //! This method allows a line to be extended by an '\' backslash at the
    //! end of the line. Note that leading and trailing whitespaces of continued
    //! lines will be trimmed before the lines are concatenated.
    //! @throws     std::runtime_error  Thrown when the stream is bad or ends unexpectedly.
    //! @returns    Returns the read concatenated lines and the number of lines read
    std::pair<std::string, int> read_line_ext(std::istream& input);

    //! Property of an ini-file.
    //! Properties have a name (string) and a value.
    struct property {
        std::string name{};     //!< name of the property
        std::string value{};    //!< value for the property (maybe empty)
        int line_number{};     //!< line number for diagnostics
    };

    //! Section of an ini-file
    //! Sections are named and can have a value like an ini-file property.
    //! Furthermore a section has an ordered list of properties
    struct section : public property {
        std::vector<property> properties{};
    };

    //! Represent an ini-file parsed from an istream.
    class file {
    public:
        file();
        explicit file(std::istream& str, bool remove_value_quotes = true);
        ~file();

        file(file const&) = default;
        file& operator=(file const&) = default;

        //! Returns the vector of all sections
        std::vector<section> const& sections() const;

    private:
        std::vector<section> _sections{};

        template<typename T> friend file merge_files(T const&);
    };

//    template<typename T>
//    file merge_files(T const& list_of_files);

    class parse_exception : public std::exception
    {
    public:
        explicit parse_exception(int line_number, std::string expr = std::string{});
        ~parse_exception() override;

        const char* what() const noexcept override;

        std::string const& expression() const noexcept;

        int line_number() const noexcept;

    private:
        std::string _expression;
        int _line_number;
    };

}

template<typename T>
core::ini::file core::ini::merge_files(T const& list_of_files)
{
    file merged{};
    section root_section{};
    for (auto const& f : list_of_files) {
        if (!f._sections.empty() && f._sections[0].name.empty()) {
            root_section.properties.insert(
                    root_section.properties.end(),
                    f._sections[0].properties.begin(),
                    f._sections[0].properties.end()
            );
        }
    }
    if (!root_section.properties.empty()) {
        merged._sections.push_back(root_section);
    }
    for (auto const& f : list_of_files) {
        for (auto const& s : f._sections) {
            if (!s.name.empty()) {
                merged._sections.push_back(s);
            }
        }
    }
    return merged;
}
