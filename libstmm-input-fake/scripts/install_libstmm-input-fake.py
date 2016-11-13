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

# File:   install_libstmm-input-fake.py

# Compiles and installs the libstmm-input-fake header-only library.

import sys
import os
import subprocess

def main():
	import argparse
	oParser = argparse.ArgumentParser()
	oParser.add_argument("-b", "--buildtype", help="build type (default=Release)"\
						, choices=['Debug', 'Release', 'MinSizeRel', 'RelWithDebInfo']\
						, default="Release", dest="sBuildType")
	oParser.add_argument("-t", "--tests", help="build tests (default=Cache)", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildTests")
	oParser.add_argument("-d", "--docs", help="build documentation (default=Cache)", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildDocs")
	oParser.add_argument("--docs-to-log", help="--docs warnings to log file", action="store_true"\
						, default=False, dest="bDocsWarningsToLog")
	oParser.add_argument("--destdir", help="install dir (default=/usr/local)", metavar='DESTDIR'\
						, default="/usr/local", dest="sDestDir")
	oParser.add_argument("--no-sudo", help="don't use sudo to install", action="store_true"\
						, default=False, dest="bDontSudo")
	oArgs = oParser.parse_args()

	sDestDir = os.path.abspath(oArgs.sDestDir)

	sScriptDir = os.path.dirname(os.path.abspath(__file__))
	#print("sScriptDir:" + sScriptDir)
	os.chdir(sScriptDir)
	os.chdir("..")
	#
	sBuildTests = " -D BUILD_TESTING="
	if oArgs.sBuildTests == "On":
		sBuildTests += "ON "
	elif oArgs.sBuildTests == "Off":
		sBuildTests += "OFF "
	else:
		sBuildTests = ""
	#print("sBuildTests:" + sBuildTests)
	#
	sBuildDocs = " -D BUILD_DOCS="
	if oArgs.sBuildDocs == "On":
		sBuildDocs += "ON "
	elif oArgs.sBuildDocs == "Off":
		sBuildDocs += "OFF "
	else:
		sBuildDocs = ""
	#print("sBuildDocs:" + sBuildDocs)
	#
	sDocsWarningsToLog = " -D BUILD_DOCS_WARNINGS_TO_LOG_FILE="
	if oArgs.bDocsWarningsToLog:
		sDocsWarningsToLog += "ON "
	else:
		sDocsWarningsToLog += "OFF "
	#print("sDocsWarningsToLog:" + sDocsWarningsToLog)
	#
	sDestDir = " -D CMAKE_INSTALL_PREFIX=" + sDestDir
	#print("sDestDir:" + sDestDir)
	#
	sBuildType = " -D CMAKE_BUILD_TYPE=" + oArgs.sBuildType
	#print("sBuildType:" + sBuildType)

	if oArgs.bDontSudo:
		sSudo = ""
	else:
		sSudo = "sudo"

	if not os.path.isdir("build"):
		os.mkdir("build")

	os.chdir("build")

	subprocess.check_call("cmake {} {} {} {} {} ..".format(sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir).split())
	subprocess.check_call("make".split())

	# The following is (probably) necessary because it's a header-only library
	oProc = subprocess.Popen("cmake -N -LA".split(), stdout=subprocess.PIPE, shell=False)
	(sOut, sErr) = oProc.communicate()
	if "BUILD_DOCS:BOOL=ON" in str(sOut):
		subprocess.check_call("make doc".split())

	subprocess.check_call("{} make install".format(sSudo).split())
	#if not oArgs.bDontSudo:
	#	subprocess.check_call("sudo ldconfig".split())


if __name__ == "__main__":
	main()
