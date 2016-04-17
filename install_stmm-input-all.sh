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

# File:   install_stmm-input-all.sh

# Compiles and installs all the projects (binaries) contained in this source package
# taking into account the ordering of their dependencies.

# The first parameter is the build type (see CMAKE_BUILD_TYPE):
#   either Debug, Release, MinSizeRel or RelWithDebInfo
# The second parameter is the (optional) destination directory:
#   if not set cmake's default value CMAKE_INSTALL_PREFIX of each project is used.

echo == install libstmm-input ==============================
cd libstmm-input                           || exit $?
./install_libstmm-input.sh $1 $2           || exit $?
cd ..

echo == install libstmm-input-ev ===========================
cd libstmm-input-ev                        || exit $?
./install_libstmm-input-ev.sh $1 $2        || exit $?
cd ..

echo == install libstmm-input-base =========================
# Note:
# libstmm-input-base doesn't depend on libstmm-input-ev but
# its test cases do.
cd libstmm-input-base                      || exit $?
./install_libstmm-input-base.sh $1 $2      || exit $?
cd ..

echo == install libstmm-input-gtk ==========================
cd libstmm-input-gtk                       || exit $?
./install_libstmm-input-gtk.sh $1 $2       || exit $?
cd ..

echo == install device-floater =============================
cd device-floater                          || exit $?
./install_device-floater.sh $1 $2          || exit $?
cd ..

