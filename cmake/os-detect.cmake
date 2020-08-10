# wirectrl is a daemon for systemd to control GPIO ports of raspberry pi
# Copyright (C) 2020 Alexander Seifarth
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# This script performs some basic checks regarding Linux distribution and kernel
# version and issues appropriate warnings to the user at CMake configure time.
if(NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(FATAL_ERROR "wirectrl is not built for other OSs than Linux")
endif()

execute_process(
    COMMAND /usr/bin/env bash ${CMAKE_CURRENT_LIST_DIR}/detect_distro.sh
    OUTPUT_VARIABLE OS_LINUX_DISTRO
    ERROR_VARIABLE  OS_RELEASE_ERROR
)

if (OS_RELEASE_ERROR)
    message(WARNING "Unable to read Linux distribution variant")
endif()

if (OS_LINUX_DISTRO MATCHES "debian")
    message(STATUS "Linux distribution: Debian")
elseif(OS_LINUX_DISTRO MATCHES "raspbian")
    message(STATUS "Linux distribution: Pi OS (raspbian)")
else()
    message(WARNING "Linux distribution unknown: ${OS_LINUX_DISTRO}")
    message(WARNING "wirectrl is supported only on Debian and Pi OS (raspbian)")
endif()

execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_VERSION
    ERROR_VARIABLE  KERNEL_VERSION_ERROR
)

set(MINIMUM_KERNEL_VERSION "4.8")
if (KERNEL_VERSION_ERROR)
    message(FATAL_ERROR "Unable to detect Linux kernel version.")
elseif(KERNEL_VERSION VERSION_LESS MINIMUM_KERNEL_VERSION)
    message(WARNING "Linux kernel version (${KERNEL_VERSION}) lower than supported. Minimum is ${MINIMUM_KERNEL_VERSION}")
else()
    message(STATUS "Linux kernel version: ${KERNEL_VERSION}")
endif()
