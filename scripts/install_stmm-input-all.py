#!/usr/bin/env python3

#  Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
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
	oParser = argparse.ArgumentParser(description="Install all stmm-input projects.\n"
						"  Option -s=On implies --omit-plugins,\n"
						"  option --omit-gtk implies --omit-x11.\n"
						"  Currently only CXX=g++ can build plugins."
						, formatter_class=argparse.RawDescriptionHelpFormatter)
	oParser.add_argument("-s", "--staticlib", help="build static libraries (instead of shared)", choices=['On', 'Off', 'Cache']\
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
	oParser.add_argument("--omit-gtk", help="do not build gtk dependant projects", action="store_true"\
						, default=False, dest="bOmitGtk")
	oParser.add_argument("--omit-x11", help="do not build x11 dependant projects", action="store_true"\
						, default=False, dest="bOmitX11")
	oParser.add_argument("--omit-plugins", help="do not build stmm-input-dl project", action="store_true"\
						, default=False, dest="bOmitPlugins")
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
	if (oArgs.sBuildStaticLib == "On"):
		oArgs.bOmitPlugins = True
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
	if oArgs.bOmitPlugins:
		sOmitPlugins = "--omit-plugins"
	else:
		sOmitPlugins = ""
	#print("sOmitPlugins:" + sOmitPlugins)
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

	if not oArgs.bOmitPlugins:
		print("== install libstmm-input-dl ==========" + sInfo + "==")
		os.chdir("libstmm-input-dl/scripts")
		subprocess.check_call("./install_libstmm-input-dl.py {} {} {} {} {} {} {} {}".format(\
				sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize).split())
		os.chdir("../..")

	print("== install libstmm-input-ev ========" + sInfo + "==")
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
		subprocess.check_call("./install_libstmm-input-gtk.py {} {} {} {} {} {} {} {}".format(\
				sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize\
				).split())
		os.chdir("../..")

		print("== install libstmm-input-gtk-dm =======" + sInfo + "==")
		os.chdir("libstmm-input-gtk-dm/scripts")
		subprocess.check_call("./install_libstmm-input-gtk-dm.py {} {} {} {} {} {} {} {} {} {}".format(\
				sBuildStaticLib, sBuildTests, sBuildDocs, sDocsWarningsToLog, sBuildType, sDestDir, sSudo, sSanitize\
				, sOmitX11, sOmitPlugins).split())
		os.chdir("../..")

		if not oArgs.bOmitX11:
			print("== install device-floater ==========" + sInfo + "==")
			os.chdir("device-floater/scripts")
			subprocess.check_call("./install_device-floater.py {} {} {}".format(sBuildType, sDestDir, sSudo).split())
			os.chdir("../..")


if __name__ == "__main__":
	main()

