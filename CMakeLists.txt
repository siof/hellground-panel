#
#    HG Players Panel - web panel for HellGround server Players
#    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License version 3 as
#    published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.8)

project("HellGround Panel")

message(
"\nThis scripts builds HellGround Panel
You can use following options for customization:
    CMAKE_INSTALL_PREFIX: Path where the server should be installed to (default - ${CMAKE_INSTALL_PREFIX})
    USE_HTTP: Uses wthttp to linking instead of wtfcgi (default - OFF)

For example: cmake -DCMAKE_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"
             cmake -DUSE_HTTP=ON\n"
)

option(USE_HTTP "Uses wthttp to linking instead of wtfcgi" OFF)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
)

find_package(Wt REQUIRED)
find_package(MySQL REQUIRED)
find_package(JWSMTP REQUIRED)
find_package(CURL REQUIRED)

set(Boost_USE_MULTITHREADED  ON)
find_package(Boost 1.34.0 COMPONENTS regex signals REQUIRED)

add_subdirectory(src)

message("")

if (USE_HTTP)
    message("*** Using Wthttp server\n")
else (USE_HTTP)
    message("*** Using Wtfcgi server\n")
endif (USE_HTTP)

message("Install directory: ${CMAKE_INSTALL_PREFIX}\n\n")
install(DIRECTORY res langs DESTINATION .
    FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
)
