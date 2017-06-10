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

# File:   checkgtkexample.py

# Checks the example of the libstmm-input-gtk README compiles.

import sys
import os
import tempfile
import subprocess


def main():

	print("Compiling libstmm-input-gtk/README.md example")
	sScriptDir = os.path.dirname(os.path.abspath(__file__))

	os.chdir(sScriptDir)
	os.chdir("..")
	sReadMeFile = os.path.abspath("libstmm-input-gtk/README.md")

	try:
		oF = open(sReadMeFile, 'r')
	except PermissionError:
		raise RuntimeError("Could not read file: " + sReadMeFile)

	aProgLines = []
	bProgStarted = False
	bProgEnded = False
	sCompileCommand = ""
	sExampleFileName = "printkey.cc"
	for sLine in oF:
		if ("// " + sExampleFileName) in sLine:
			bProgStarted = True
		if bProgStarted and not bProgEnded:
			if "From the terminal:" in sLine:
				bProgEnded = True
			else:
				aProgLines.append(sLine)
		if bProgEnded:
			if ("g++ " + sExampleFileName) in sLine:
				sCompileCommand = sLine
				break
	else:
		raise RuntimeError("Could not extract example from file: " + sPathFile)

	nPos = sCompileCommand.find(sExampleFileName)
	if nPos < 0:
		raise RuntimeError("Fatal error")
	(oDummy, sDestPath) = tempfile.mkstemp(suffix='.cc', prefix='tmp', text=True)

	(sDestDir, sDestName) = os.path.split(sDestPath)
	os.chdir(sDestDir)
	sCompileCommand = sCompileCommand[:nPos] + sDestName + sCompileCommand[nPos + len(sExampleFileName):]

	oFDest = open(sDestPath, 'w')
	for sLine in aProgLines:
		oFDest.write(sLine)
	oFDest.close()

	sCompileCommand = sCompileCommand.strip()
	#print(sDestPath)
	#print(aProgLines)
	#print(sCompileCommand)

	subprocess.check_call(sCompileCommand, shell=True)

	os.remove(sDestPath)
	print("Compiled libstmm-input-gtk/README.md example")


if __name__ == "__main__":
	main()
