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

# File:   testall.py

# - compiles and installs all the projects (binaries) contained in this source package
#   for both Debug and Release configurations and for both shared and static libs
# - compiles the examples
# - creates documentation and checks it is ok
# - runs all tests
# - makes sure no line in the code starts with std::cout
# - optionally calls clang-llvm sanitizer
# - optionally calls clang-tidy

import sys
import os
import subprocess


def testAll(sBuildType, sOmitGtk, sOmitX11, sOmitPlugins, sDestDir, sSudo, bBuildStatic, bSanitize):
	if bBuildStatic:
		sOmitPlugins = "--omit-plugins"

	sStatic = ""
	if bBuildStatic:
		sStatic = "-s On"
	else:
		sStatic = "-s Off"

	if bSanitize:
		sSanitize = "--sanitize"
		sSanitizeOptions = "ASAN_OPTIONS=check_initialization_order=1:strict_init_order=1:detect_odr_violation=1"
		# ":alloc_dealloc_mismatch=1:detect_leaks=1:allow_user_segv_handler=1:new_delete_type_mismatch=1
		sSanitizeIgnoreOptions = ":detect_leaks=0:new_delete_type_mismatch=0"
	else:
		sSanitize = ""
		sSanitizeOptions = ""
		sSanitizeIgnoreOptions = ""

	subprocess.check_call("./scripts/uninstall_stmm-input-all.py -r -y --destdir {}  {}".format(sDestDir, sSudo).split())

	subprocess.check_call("./scripts/install_stmm-input-all.py -b {}  -t On  -d On --docs-to-log  --destdir {}  {} {} {}"\
			" {} {} {}"\
			.format(sBuildType, sDestDir, sSudo, sStatic, sSanitize, sOmitGtk, sOmitX11, sOmitPlugins).split())


	os.chdir("libstmm-input/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("libstmm-input_doxy.log") > 0:
		raise RuntimeError("Error: libstmm-input_doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-base/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("libstmm-input-base_doxy.log") > 0:
		raise RuntimeError("Error: libstmm-input-base_doxy.log not empty")
	os.chdir("../..")

	if sOmitPlugins == "":
		print("----->>>>" + sOmitPlugins + "  " + str(bBuildStatic))
		os.chdir("libstmm-input-dl/build")
		subprocess.check_call(sSanitizeOptions + " make test", shell=True)
		if os.path.getsize("libstmm-input-dl_doxy.log") > 0:
			raise RuntimeError("Error: libstmm-input-dl_doxy.log not empty")
		os.chdir("../..")

	os.chdir("libstmm-input-ev/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("libstmm-input-ev_doxy.log") > 0:
		raise RuntimeError("Error: libstmm-input-ev_doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-fake/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("libstmm-input-fake_doxy.log") > 0:
		raise RuntimeError("Error: libstmm-input-fake_doxy.log not empty")
	os.chdir("../..")

	if (sOmitGtk == ""):
		os.chdir("libstmm-input-gtk/build")
		subprocess.check_call(sSanitizeOptions + sSanitizeIgnoreOptions + " make test", shell=True)
		if os.path.getsize("libstmm-input-gtk_doxy.log") > 0:
			raise RuntimeError("Error: libstmm-input-gtk_doxy.log not empty")
		os.chdir("../..")

		os.chdir("libstmm-input-gtk-dm/build")
		subprocess.check_call(sSanitizeOptions + sSanitizeIgnoreOptions + " make test", shell=True)
		if os.path.getsize("libstmm-input-gtk-dm_doxy.log") > 0:
			raise RuntimeError("Error: libstmm-input-gtk-dm_doxy.log not empty")
		os.chdir("../..")

	if (not bSanitize) and (sOmitGtk == ""):
		os.chdir("libstmm-input-gtk-dm/examples/bare-app")
		if not os.path.isdir("build"):
			os.mkdir("build")
		os.chdir("build")
		subprocess.check_call("cmake -D CMAKE_BUILD_TYPE={} ..".format(sBuildType).split())
		subprocess.check_call("make".split())
		os.chdir("../../../..")

		os.chdir("libstmm-input-gtk-dm/examples/showevs")
		if not os.path.isdir("build"):
			os.mkdir("build")
		os.chdir("build")
		subprocess.check_call("cmake -D CMAKE_BUILD_TYPE={} ..".format(sBuildType).split())
		subprocess.check_call("make".split())
		os.chdir("../../../..")

		os.chdir("libstmm-input-fake/examples/spinn")
		if not os.path.isdir("build"):
			os.mkdir("build")
		os.chdir("build")
		subprocess.check_call("cmake -D CMAKE_BUILD_TYPE={} -D BUILD_TESTING=ON ..".format(sBuildType).split())
		subprocess.check_call("make".split())
		subprocess.check_call("make test".split())
		os.chdir("../../../..")

	if (not bBuildStatic) and (sOmitGtk == ""):
		subprocess.check_call("./scripts/checkgtkexample.py".split())


def checkTidy(sOmitGtk, sOmitX11, sOmitPlugins):
	#
	print("-------checkTidy " + sOmitPlugins)
	subprocess.check_call("./scripts/checktidy.py {} {} {}".format(sOmitGtk, sOmitX11, sOmitPlugins).split())
	

def main():

	import argparse
	oParser = argparse.ArgumentParser(description="Uninstall, compile, document, reinstall and test all projects\n"
									"  Option -s=On implies --omit-plugins,\n"
									"  option --omit-gtk implies --omit-x11.\n"
									"Currently only CXX=g++ can build plugins."
									, formatter_class=argparse.RawDescriptionHelpFormatter)
	oParser.add_argument("-y", "--no-prompt", help="no prompt comfirmation", action="store_true"\
						, default=False, dest="bNoPrompt")
	oParser.add_argument("--omit-gtk", help="do not build gtk dependant projects", action="store_true"\
						, default=False, dest="bOmitGtk")
	oParser.add_argument("--omit-x11", help="do not build x11 dependant projects", action="store_true"\
						, default=False, dest="bOmitX11")
	oParser.add_argument("--omit-plugins", help="do not build stmm-input-dl project", action="store_true"\
						, default=False, dest="bOmitPlugins")
	oParser.add_argument("--destdir", help="install dir (default=/usr/local)", metavar='DESTDIR'\
						, default="/usr/local", dest="sDestDir")
	oParser.add_argument("--no-sudo", help="don't use sudo to (un)install", action="store_true"\
						, default=False, dest="bDontSudo")
	oParser.add_argument("-b", "--buildtype", help="build type (default=Both)", choices=['Debug', 'Release', 'Both']\
						, default="Both", dest="sBuildType")
	oParser.add_argument("-l", "--link", help="build static library or shared", choices=['Static', 'Shared', 'Both']\
						, default="Both", dest="sLinkType")
	oParser.add_argument("--sanitize", help="execute tests with llvm address sanitize checks (Debug+Static only)", action="store_true"\
						, default=False, dest="bSanitize")
	oParser.add_argument("--tidy", help="apply clang-tidy to source code (CXX=clang++ only)", action="store_true"\
						, default=False, dest="bTidy")
	oArgs = oParser.parse_args()

	while not oArgs.bNoPrompt:
		sAnswer = input("Are you sure? (yes/no) >").strip()
		if sAnswer.casefold() == "yes":
			break
		elif sAnswer.casefold() == "no":
			return #-----------------------------------------------------------

	sDestDir = os.path.abspath(oArgs.sDestDir)

	sScriptDir = os.path.dirname(os.path.abspath(__file__))
	os.chdir(sScriptDir)
	os.chdir("..")

	subprocess.check_call("./scripts/checkcode.py".split())

	if oArgs.bDontSudo:
		sSudo = "--no-sudo"
	else:
		sSudo = ""

	if oArgs.bOmitGtk:
		sOmitGtk = "--omit-gtk"
		oArgs.bOmitX11 = True
	else:
		sOmitGtk = ""

	if oArgs.bOmitX11:
		sOmitX11 = "--omit-x11"
	else:
		sOmitX11 = ""

	if oArgs.bOmitPlugins:
		sOmitPlugins = "--omit-plugins"
	else:
		sOmitPlugins = ""

	bTidyDone = False  # tidy should be applied only once for the same build type

	if (oArgs.sLinkType == "Both") or (oArgs.sLinkType == "Static"):
		bBuildStatic = True
		if oArgs.sBuildType == "Both":
			if oArgs.bSanitize:
				testAll("Debug", sOmitGtk, sOmitX11, "--omit-plugins", sDestDir, sSudo, bBuildStatic, True)
			#
			testAll("Debug", sOmitGtk, sOmitX11, "--omit-plugins", sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy:
				checkTidy(sOmitGtk, sOmitX11, "--omit-plugins")
			#
			testAll("Release", sOmitGtk, sOmitX11, "--omit-plugins", sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy:
				checkTidy(sOmitGtk, sOmitX11, "--omit-plugins")
				bTidyDone = True
		else:
			if oArgs.bSanitize and (oArgs.sBuildType == "Debug"):
				testAll("Debug", sOmitGtk, sOmitX11, "--omit-plugins", sDestDir, sSudo, bBuildStatic, True)
			#
			testAll(oArgs.sBuildType, sOmitGtk, sOmitX11, "--omit-plugins", sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy:
				checkTidy(sOmitGtk, sOmitX11, "--omit-plugins")
				bTidyDone = True

	if (oArgs.sLinkType == "Both") or (oArgs.sLinkType == "Shared"):
		bBuildStatic = False
		if oArgs.sBuildType == "Both":
			testAll("Debug", sOmitGtk, sOmitX11, sOmitPlugins, sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy and not bTidyDone:
				checkTidy(sOmitGtk, sOmitX11, sOmitPlugins)
			#
			testAll("Release", sOmitGtk, sOmitX11, sOmitPlugins, sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy and not bTidyDone:
				checkTidy(sOmitGtk, sOmitX11, sOmitPlugins)
		else:
			testAll(oArgs.sBuildType, sOmitGtk, sOmitX11, sOmitPlugins, sDestDir, sSudo, bBuildStatic, False)
			if oArgs.bTidy and not bTidyDone:
				checkTidy(sOmitGtk, sOmitX11, sOmitPlugins)

	print("---------------------------------")
	print("testall.py finished successfully!")
	print("---------------------------------")


if __name__ == "__main__":
	main()

