#! /bin/sh

#  Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 3 of the License, or (at your option) any later version.
# 
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, see <http://www.gnu.org/licenses/>

# File:   install_libstmm-input-base.sh


# Compiles and installs the libstmm-input-base library.

# The first parameter is the BUILD_TYPE (optional):
#   either Debug, Release, MinSizeRel or RelWithDebInfo.
# The second parameter is the (optional) destination directory:
#   if not set cmake's default value for CMAKE_INSTALL_PREFIX is used.
# All other cmake options can be set with programs such as cmake-gui.

# if "build" exists and is a directory
if [ -d build ]
then
  echo
else
  mkdir build                                   || exit $?
fi

cd build                                        || exit $?

# if second parameter not empty
if [ -n "$2" ]
then
  cmake -D CMAKE_BUILD_TYPE=$1         \
        -D CMAKE_INSTALL_PREFIX=$2     \
        ..                                      || exit $?
else
  cmake -D CMAKE_BUILD_TYPE=$1         \
        ..                                      || exit $?
fi

make                                            || exit $?
sudo make install                               || exit $?

sudo ldconfig
