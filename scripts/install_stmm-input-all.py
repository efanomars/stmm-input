#!/usr/bin/env python3

#  Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
	oParser.add_argument("-s", "--staticlib", help="build static library (instead of shared)", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildStaticLib")
	oParser.add_argument("-b", "--buildtype", help="build type (default=Release)"\
						, choices=['Debug', 'Release', 'MinSizeRel', 'RelWithDebInfo']\
						, default="Release", dest="sBuildType")
	oParser.add_argument("-t", "--tests", help="build tests (default=Cache)", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildTests")
	oParser.add_argument("-d", "--docs", help="build documentation (default=Cache)", choices=['On', 'Off', 'Cache']\
						, default="Cache", dest="sBuildDocs")
	oParser.add_argument("--docs-to-log", help="--docs warnings to log file", action="store_true"\
						, default=False, dest="bDocsWarningsToLog")
	oParser.add_argument("--omit-gtk", help="do not compile gtk dependant projects", action="store_true"\
						, default=False, dest="bOmitGtk")
	oParser.add_argument("--omit-x11", help="do not compile x11 dependant projects", action="store_true"\
						, default=False, dest="bOmitX11")
	oParser.add_argument("--destdir", help="install dir (default=/usr/local)", metavar='DESTDIR'\
						, default="/usr/local", dest="sDestDir")
	oParser.add_argument("--no-sudo", help="don't use sudo to install", action="store_true"\
						, default=False, dest="bDontSudo")
	oParser.add_argument("--sanitize", help="compile libraries with -fsanitize=address (Debug only)", action="store_true"\
						, default=False, dest="bSanitize")
	oArgs = oParser.parse_args()

	sDestDir = os.path.abspath(oArgs.sDestDir)

	sScriptDir = os.path.dirname(os.path.abspath(__file__))
	#print("sScriptDir:" + sScriptDir)
	os.chdir(sScriptDir)
	os.chdir("..") # change to source dir

	#
	sBuildStaticLib = "-s " + oArgs.sBuildStaticLib
	#print("sBuildStaticLib:" + sBuildStaticLib)
	#
	sBuildTests = "-t " + oArgs.sBuildTests
	#print("sBuildTests:" + sBuildTests)
	if oArgs.bOmitGtk:
		oArgs.bOmitX11 = True
	if oArgs.bOmitX11:
		sOmitX11 = "--omit-x11"
	else:
		sOmitX11 = ""
	#print("sOmitX11:" + sOmitX11)
	#
	sBuildDocs = "-d " + oArgs.sBuildDocs
	#print("sBuildDocs:" + sBuildDocs)
	#
	if oArgs.bDocsWarningsToLog:
		sDocsWarningsToLog = "--docs-to-log"
	else:
		sDocsWarningsToLog = ""
	#
	sDestDir = "--destdir " + sDestDir
	#print("sDestDir:" + sDestDir)
	#
	sBuildType = "-b " + oArgs.sBuildType
	#print("sBuildType:" + sBuildType)
	#
	if oArgs.bSanitize:
		sSanitize = "--sanitize"
	else:
		sSanitize = ""
	#print("sSanitize:" + sSanitize)

	#
	if oArgs.bDontSudo:
		sSudo = "--no-sudo"
	else:
		sSudo = ""

	sInfo = " " + oArgs.sBuildType + " == static " + oArgs.sBuildStaticLib + " "

	print("== install libstmm-input =============" + sInfo + "==")
	os.chdir("libstmm-input/scripts")
	subprocess.check_call("./install_libstmm-input.py {} {} {} {} {} {} {} {}".format(\
			sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize).split())
	os.chdir("../..")

	print("== install libstmm-input-base ========" + sInfo + "==")
	os.chdir("libstmm-input-base/scripts")
	subprocess.check_call("./install_libstmm-input-base.py {} {} {} {} {} {} {} {}".format(\
			sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize).split())
	os.chdir("../..")

	print("== install libstmm-input-ev ==========" + sInfo + "==")
	os.chdir("libstmm-input-ev/scripts")
	subprocess.check_call("./install_libstmm-input-ev.py {} {} {} {} {} {} {} {}".format(\
			sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize).split())
	os.chdir("../..")

	print("== install libstmm-input-fake ========" + sInfo + "==")
	os.chdir("libstmm-input-fake/scripts")
	subprocess.check_call("./install_libstmm-input-fake.py {} {} {} {} {} {} {} {}".format(\
			sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize).split())
	os.chdir("../..")

	if not oArgs.bOmitGtk:
		print("== install libstmm-input-gtk =======" + sInfo + "==")
		os.chdir("libstmm-input-gtk/scripts")
		subprocess.check_call("./install_libstmm-input-gtk.py {} {} {} {} {} {} {} {} {}".format(\
				sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize, sOmitX11).split())
		os.chdir("../..")

		if not oArgs.bOmitX11:
			print("== install device-floater ==========" + sInfo + "==")
			os.chdir("device-floater/scripts")
			subprocess.check_call("./install_device-floater.py {} {} {}".format(sBuildType, sDestDir, sSudo).split())
			os.chdir("../..")


if __name__ == "__main__":
	main()

