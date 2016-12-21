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

# File:   testall.py

# - compiles and installs all the projects (binaries) contained in this source package
#   for both Debug and Release configurations and for both shared and static libs
# - compiles the examples
# - creates documentation and checks it is ok
# - runs all tests
# - makes sure no line in the code start with std::cout

import sys
import os
import subprocess


def testAll(sBuildType, sDestDir, sSudo, sStatic, bSanitize):
	if bSanitize:
		sSanitize = "--sanitize"
		sSanitizeOptions = "ASAN_OPTIONS=check_initialization_order=1:strict_init_order=1:detect_odr_violation=1"
		# ":alloc_dealloc_mismatch=1:detect_leaks=1:allow_user_segv_handler=1:new_delete_type_mismatch=1
		sSanitizeIgnoreLeaksOptions = ":detect_leaks=0:new_delete_type_mismatch=0"
	else:
		sSanitize = ""
		sSanitizeOptions = ""
		sSanitizeIgnoreLeaksOptions = ""

	subprocess.check_call("./scripts/uninstall_stmm-input-all.py -r -y --destdir {}  {}".format(sDestDir, sSudo).split())

	subprocess.check_call("./scripts/install_stmm-input-all.py -b {}  -t On  -d On --docs-to-log  --destdir {}  {} {} {}"\
			.format(sBuildType, sDestDir, sSudo, sStatic, sSanitize).split())


	os.chdir("libstmm-input/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("stmm-input-doxy.log") > 0:
		raise RuntimeError("Error: stmm-input-doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-base/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("stmm-input-base-doxy.log") > 0:
		raise RuntimeError("Error: stmm-input-base-doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-ev/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("stmm-input-ev-doxy.log") > 0:
		raise RuntimeError("Error: stmm-input-ev-doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-fake/build")
	subprocess.check_call(sSanitizeOptions + " make test", shell=True)
	if os.path.getsize("stmm-input-fake-doxy.log") > 0:
		raise RuntimeError("Error: stmm-input-fake-doxy.log not empty")
	os.chdir("../..")

	os.chdir("libstmm-input-gtk/build")
	subprocess.check_call(sSanitizeOptions + sSanitizeIgnoreLeaksOptions + " make test", shell=True)
	if os.path.getsize("stmm-input-gtk-doxy.log") > 0:
		raise RuntimeError("Error: stmm-input-gtk-doxy.log not empty")
	os.chdir("../..")

	if not bSanitize:
		os.chdir("libstmm-input-gtk/examples/bare-app")
		if not os.path.isdir("build"):
			os.mkdir("build")
		os.chdir("build")
		subprocess.check_call("cmake -D CMAKE_BUILD_TYPE={} ..".format(sBuildType).split())
		subprocess.check_call("make".split())
		os.chdir("../../../..")

		os.chdir("libstmm-input-gtk/examples/showevs")
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

	if sStatic == "-s Off":
		subprocess.check_call("./scripts/checkgtkexample.py".split())



def main():

	import argparse
	oParser = argparse.ArgumentParser(description="Uninstall, compile, document, reinstall and test all projects")
	oParser.add_argument("-y", "--no-prompt", help="no prompt comfirmation", action="store_true"\
						, default=False, dest="bNoPrompt")
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

	if (oArgs.sLinkType == "Both") or (oArgs.sLinkType == "Static"):
		if oArgs.sBuildType == "Both":
			if oArgs.bSanitize:
				testAll("Debug", sDestDir, sSudo, "-s On", True)
			testAll("Debug", sDestDir, sSudo, "-s On", False)
			testAll("Release", sDestDir, sSudo, "-s On", False)
		else:
			if oArgs.bSanitize and (oArgs.sBuildType == "Debug"):
				testAll("Debug", sDestDir, sSudo, "-s On", True)
			testAll(oArgs.sBuildType, sDestDir, sSudo, "-s On", False)

	if (oArgs.sLinkType == "Both") or (oArgs.sLinkType == "Shared"):
		if oArgs.sBuildType == "Both":
			testAll("Debug", sDestDir, sSudo, "-s Off", False)
			testAll("Release", sDestDir, sSudo, "-s Off", False)
		else:
			testAll(oArgs.sBuildType, sDestDir, sSudo, "-s Off", False)

	print("---------------------------------")
	print("testall.py finished successfully!")
	print("---------------------------------")


if __name__ == "__main__":
	main()

