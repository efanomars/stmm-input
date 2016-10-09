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

# File:   install_stmm-input-all.py

# Compiles and installs all the projects (binaries) contained in this source package
# taking into account the ordering of their dependencies.

import sys
import os
import subprocess

def main():
	import argparse
	oParser = argparse.ArgumentParser()
	oParser.add_argument("-b", "--buildtype", help="build type", choices=['Debug', 'Release', 'MinSizeRel', 'RelWithDebInfo']\
						, default="Release", dest="sBuildType")
	oParser.add_argument("-t", "--tests", help="build tests", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildTests")
	oParser.add_argument("-d", "--docs", help="build documentation", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildDocs")
	oParser.add_argument("--docs-to-log", help="--docs warnings to log file", action="store_true", dest="bDocsWarningsToLog")
	oParser.add_argument("--destdir", help="destination dir", metavar='DESTDIR', default="/usr/local", dest="sDestDir")
	oArgs = oParser.parse_args()

	#sSourceDir = os.path.dirname(os.path.abspath(__file__))
	#print("sSourceDir:" + sSourceDir)
	#
	sBuildTests = "-t " + oArgs.sBuildTests
	#print("sBuildTests:" + sBuildTests)
	#
	sBuildDocs = "-d " + oArgs.sBuildDocs
	#print("sBuildDocs:" + sBuildDocs)
	#
	if oArgs.bDocsWarningsToLog:
		sDocsWarningsToLog = "--docs-to-log"
	else:
		sDocsWarningsToLog = ""
	#
	sDestDir = "--destdir " + oArgs.sDestDir
	#print("sDestDir:" + sDestDir)
	#
	sBuildType = "-b " + oArgs.sBuildType
	#print("sBuildType:" + sBuildType)

	print("== install libstmm-input ==============================")
	os.chdir("libstmm-input")
	subprocess.check_call("./install_libstmm-input.py {} {} {} {} {}".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	os.chdir("..")

	print("== install libstmm-input-base =========================")
	os.chdir("libstmm-input-base")
	subprocess.check_call("./install_libstmm-input-base.py {} {} {} {} {}".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	os.chdir("..")

	print("== install libstmm-input-ev ===========================")
	os.chdir("libstmm-input-ev")
	subprocess.check_call("./install_libstmm-input-ev.py {} {} {} {} {}".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	os.chdir("..")

	print("== install libstmm-input-gtk ==========================")
	os.chdir("libstmm-input-gtk")
	subprocess.check_call("./install_libstmm-input-gtk.py {} {} {} {} {}".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	os.chdir("..")

	print("== install libstmm-input-fake =========================")
	os.chdir("libstmm-input-fake")
	subprocess.check_call("./install_libstmm-input-fake.py {} {} {} {} {}".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	os.chdir("..")

	print("== install device-floater =============================")
	os.chdir("device-floater")
	subprocess.check_call("./install_device-floater.py {} {}".format(sBuildType, sDestDir).split())
	os.chdir("..")


if __name__ == "__main__":
	main()

