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

find_path(JWSMTP_INCLUDE_DIR NAMES jwsmtp/jwsmtp.h PATHS ENV PATH PATH_SUFFIXES include jwsmtp jwsmtp-1.32)

find_library(JWSMTP_LIBRARY NAMES jwsmtp PATHS PATH PATH_SUFFIXES lib)

if (JWSMTP_INCLUDE_DIR AND JWSMTP_LIBRARY)
    if (NOT JWSMTP_FIND_QUIETLY)
        message(STATUS "Found the JWSMTP libraries at ${JWSMTP_LIBRARY}")
        message(STATUS "Found the JWSMTP headers at ${JWSMTP_INCLUDE_DIR}")
    endif (NOT JWSMTP_FIND_QUIETLY)
else (JWSMTP_INCLUDE_DIR AND JWSMTP_LIBRARY)
    if (JWSMTP_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find JWSMTP")
    endif (JWSMTP_FIND_REQUIRED)
endif (JWSMTP_INCLUDE_DIR AND JWSMTP_LIBRARY)
