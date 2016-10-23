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

# File:   uninstall_stmm-input-all.py

# Removes all files installed by the install_stmm-input-all.sh script.

import sys
import os
import subprocess

def main():
	import argparse
	oParser = argparse.ArgumentParser("Removes (with sudo) all files created by install_stmm-input-all.py")
	oParser.add_argument("-r", "--remove-builds", help="Remove build folders", action="store_true"\
						, default=False, dest="bRemoveBuilds")
	oParser.add_argument("-y", "--no-prompt", help="No prompt comfirmation", action="store_true"\
						, default=False, dest="bNoPrompt")
	oParser.add_argument("--destdir", help="install dir (default=/usr/local)", metavar='DESTDIR'\
						, default="/usr/local", dest="sDestDir")
	oParser.add_argument("--no-sudo", help="don't use sudo to uninstall", action="store_true"\
						, default=False, dest="bDontSudo")
	oArgs = oParser.parse_args()

	sDestDir = os.path.abspath(oArgs.sDestDir)

	sScriptDir = os.path.dirname(os.path.abspath(__file__))
	os.chdir(sScriptDir)
	os.chdir("..")

	print("Uninstall from dir: " + sDestDir + "   Remove build folders: " + str(oArgs.bRemoveBuilds))

	while not oArgs.bNoPrompt:
		sAnswer = input("Are you sure? (yes/no) >").strip()
		if sAnswer.casefold() == "yes":
			break
		elif sAnswer.casefold() == "no":
			return #-----------------------------------------------------------

	if oArgs.bRemoveBuilds:
		sRemoveBuilds = "-r"
	else:
		sRemoveBuilds = ""

	if oArgs.bDontSudo:
		sSudo = "--no-sudo"
	else:
		sSudo = ""

	subprocess.check_call("./device-floater/scripts/uninstall_device-floater.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())

	subprocess.check_call("./libstmm-input/scripts/uninstall_libstmm-input.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())

	subprocess.check_call("./libstmm-input-base/scripts/uninstall_libstmm-input-base.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())

	subprocess.check_call("./libstmm-input-ev/scripts/uninstall_libstmm-input-ev.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())

	subprocess.check_call("./libstmm-input-fake/scripts/uninstall_libstmm-input-fake.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())

	subprocess.check_call("./libstmm-input-gtk/scripts/uninstall_libstmm-input-gtk.py -y {} --destdir {}  {}"\
						.format(sRemoveBuilds, sDestDir, sSudo).split())


if __name__ == "__main__":
	main()

