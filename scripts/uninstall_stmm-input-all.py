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
	oParser = argparse.ArgumentParser()
	oParser.add_argument("-r", "--remove-builds", help="Remove build folders", action="store_true"\
			, default=False, dest="bRemoveBuilds")
	oParser.add_argument("-n", "--no-prompt", help="No prompt comfirmation", action="store_true"\
			, default=False, dest="bNoPrompt")
	oParser.add_argument("--destdir", help="install dir", metavar='DESTDIR', default="/usr/local", dest="sDestDir")
	oArgs = oParser.parse_args()

	sDestDir = oArgs.sDestDir

	print("Uninstall from dir: " + sDestDir + "   Remove build folders: " + str(oArgs.bRemoveBuilds))

	bDoIt = False
	while not oArgs.bNoPrompt:
		sAnswer = input("Are you sure? (yes/no) >").strip()
		if sAnswer.casefold() == "yes":
			bDoIt = True
			break
		elif sAnswer.casefold() == "no":
			break

	if bDoIt:
		subprocess.check_call("sudo rm -r -f {}/bin/device-floater".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f {}/share/device-floater".format(sDestDir).split())

		subprocess.check_call("sudo rm -r -f       {}/include/stmm-input".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input.so*".format(sDestDir), shell=True)
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input.a".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f {}/lib/pkgconfig/stmm-input.pc".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f     {}/share/doc/stmm-input".format(sDestDir).split())

		subprocess.check_call("sudo rm -r -f       {}/include/stmm-input-base".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-base.so*".format(sDestDir), shell=True)
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-base.a".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f {}/lib/pkgconfig/stmm-input-base.pc".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f     {}/share/doc/stmm-input-base".format(sDestDir).split())

		subprocess.check_call("sudo rm -r -f       {}/include/stmm-input-ev".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-ev.so*".format(sDestDir), shell=True)
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-ev.a".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f {}/lib/pkgconfig/stmm-input-ev.pc".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f     {}/share/doc/stmm-input-ev".format(sDestDir).split())

		subprocess.check_call("sudo rm -r -f       {}/include/stmm-input-gtk".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-gtk.so*".format(sDestDir), shell=True)
		subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-gtk.a".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f {}/lib/pkgconfig/stmm-input-gtk.pc".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f     {}/share/doc/stmm-input-gtk".format(sDestDir).split())

		subprocess.check_call("sudo rm -r -f       {}/include/stmm-input-fake".format(sDestDir).split())
		#subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-fake.so*".format(sDestDir), shell=True)
		#subprocess.check_call("sudo rm    -f        {}/lib/libstmm-input-fake.a".format(sDestDir).split())
		subprocess.check_call("sudo rm    -f {}/lib/pkgconfig/stmm-input-fake.pc".format(sDestDir).split())
		subprocess.check_call("sudo rm -r -f     {}/share/doc/stmm-input-fake".format(sDestDir).split())

		if oArgs.bRemoveBuilds:

			sScriptDir = os.path.dirname(os.path.abspath(__file__))

			os.chdir(sScriptDir)
			os.chdir("..")

			subprocess.check_call("sudo rm -r -f libstmm-input/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-base/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-ev/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-gtk/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-gtk/examples/bare-app/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-gtk/examples/showevs/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-fake/build".split())
			subprocess.check_call("sudo rm -r -f libstmm-input-fake/examples/spinn/build".split())
			subprocess.check_call("sudo rm -r -f device-floater/build".split())

		subprocess.check_call("sudo ldconfig".split())


if __name__ == "__main__":
	main()

