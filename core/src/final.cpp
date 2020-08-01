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
#include <core/final.h>

using namespace core;

final::final(std::function<void()> finalizer_)
    : _finalizer{finalizer_}
{}

final::~final()
{
    if (_finalizer) {
        _finalizer();
    }
}

void final::reset() noexcept
{
    _finalizer = std::function<void()>{};
}

void final::reset(std::function<void()> finalizer_) noexcept
{
    _finalizer = finalizer_;
}
