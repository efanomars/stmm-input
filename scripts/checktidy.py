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

# File:   checktidy.py

# Use this command to check all stmm-input related projects with clang-tidy.

import sys
import os
import subprocess

def callTidy(sSubDir):
	#
	os.chdir(sSubDir)

	print("clang-tidy started for project: " + sSubDir)

	sTidyOutput = "build/" + sSubDir + "_tidy.log"
	sCmd = ("clang-tidy -checks='.*' -p build/compile_commands.json  src/*.cc"
			" >" + sTidyOutput)
	subprocess.check_call(sCmd, shell=True)
	#
	if os.path.getsize(sTidyOutput) > 0:
		raise RuntimeError("Error: " + sTidyOutput + " not empty")

	print("clang-tidy finished project " + sSubDir + ". Output file: " + sTidyOutput)

	os.chdir("..")

def main():

	import argparse
	oParser = argparse.ArgumentParser(description="Clang tidy")
	oParser.add_argument("--omit-gtk", help="do not compile and test gtk dependant projects", action="store_true"\
						, default=False, dest="bOmitGtk")
	oParser.add_argument("--omit-x11", help="do not compile and test x11 dependant projects", action="store_true"\
						, default=False, dest="bOmitX11")
	oArgs = oParser.parse_args()

	sScriptDir = os.path.dirname(os.path.abspath(__file__))

	os.chdir(sScriptDir)
	os.chdir("..")
	#
	if not (oArgs.bOmitGtk or oArgs.bOmitX11):
		callTidy("device-floater")
	callTidy("libstmm-input")
	callTidy("libstmm-input-base")
	callTidy("libstmm-input-ev")
	callTidy("libstmm-input-fake")
	#os.chdir("libstmm-input-fake/examples")
	#callTidy("spinn")
	#os.chdir("../..")
	if not oArgs.bOmitGtk:
		callTidy("libstmm-input-gtk")


if __name__ == "__main__":
	main()

