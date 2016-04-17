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

# File:   sourcepackage.sh


# Creates a source package containing all stmm-input related projects.

exclstr="--recursion"

for projdir in libstmm-input  libstmm-input-base  libstmm-input-ev  \
               libstmm-input-gtk \
               libstmm-input-gtk/examples/bare-app \
               libstmm-input-gtk/examples/showevs  \
               device-floater
do
  for exclfile in "build*" "configure" "nbproject*" ".project" ".cproject"
  do
    exclstr="$exclstr --exclude=$projdir/$exclfile"
  done
done

directorypwd=`pwd`
directoryname=`basename $directorypwd`
dateversion=`date +%Y%m%d`

tar -zcf  ../stmm-input-$dateversion.tar.gz   -v      \
     ../$directoryname                               \
     --exclude=.git                                  \
     --exclude=stuff                                 \
     --exclude=.metadata                             \
     --exclude=libstmm-input-gtk/examples/.metadata  \
     $exclstr
