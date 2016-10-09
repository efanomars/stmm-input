#!/usr/bin/env python3

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

# File:   install_device-floater.py

# Compiles and installs the device-floater application.

import sys
import os
import subprocess

def main():
	import argparse
	oParser = argparse.ArgumentParser()
	oParser.add_argument("-b", "--buildtype", help="build type", choices=['Debug', 'Release', 'MinSizeRel', 'RelWithDebInfo'], default="Release", dest="sBuildType")
	oParser.add_argument("--destdir", help="destination dir", metavar='DESTDIR', default="/usr/local", dest="sDestDir")
	oArgs = oParser.parse_args()
	#
	sDestDir = " -D CMAKE_INSTALL_PREFIX=" + oArgs.sDestDir
	#print("sDestDir:" + sDestDir)
	#
	sBuildType = " -D CMAKE_BUILD_TYPE=" + oArgs.sBuildType
	#print("sBuildType:" + sBuildType)

	if not os.path.isdir("build"):
		os.mkdir("build")

	os.chdir("build")

	subprocess.check_call("cmake {} {} ..".format(sBuildType, sDestDir).split())
	subprocess.check_call("make".split())
	subprocess.check_call("sudo make install".split())


if __name__ == "__main__":
	main()

