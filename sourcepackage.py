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

# File:   sourcepackage.py

# Creates a source package containing all stmm-input related projects.

import sys
import os
import subprocess
from datetime import date

def main():
	aProjDirs = ["libstmm-input", "libstmm-input-base", "libstmm-input-ev"
				, "libstmm-input-fake"
				, "libstmm-input-fake/examples/spinn"
				, "libstmm-input-gtk"
				, "libstmm-input-gtk/examples/bare-app"
				, "libstmm-input-gtk/examples/showevs"
				, "device-floater"]

	aExclPatterns = ["build*", "configure", "nbproject*", ".project", ".cproject", ".settings"]

	sExcludes = ""
	for sProjDir in aProjDirs:
		for sExclPattern in aExclPatterns:
			sExcludes += " --exclude=" + os.path.join(sProjDir, sExclPattern)

	sSourceDir = os.path.basename(os.path.dirname(os.path.abspath(__file__)))

	oToday = date.today()
	sToday = ("000" + str(oToday.year))[-4:] + ("0" + str(oToday.month))[-2:] + ("0" + str(oToday.day))[-2:]

	#print("sExcludes=" + sExcludes)
	#print("sSourceDir=" + sSourceDir)
	#print("sToday=" + sToday)

	sCmd = ("tar -zcf  ../stmm-input-{}.tar.gz -v"
							" ../{}"
							" --exclude=.git"
							" --exclude=stuff"
							" --exclude=.metadata"
							" --exclude=libstmm-input-fake/examples/.metadata"
							" --exclude=libstmm-input-gtk/examples/.metadata"
							" {}").format(sToday, sSourceDir, sExcludes)
	#print(sCmd)

	subprocess.check_call(sCmd.split())


if __name__ == "__main__":
	main()

