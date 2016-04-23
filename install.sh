#!/bin/bash
# This script should be able to set up all dependencies for rpgxEF development
# on a brand new Ubuntu machine. Please let Telex Ferra know if it isn't working.

if [ $(whoami) != 'root' ];     
	then 
	echo "Please run this script as root."
	exit -1
fi

cd ~ || exit -1
mkdir -p Projects
git clone https://github.com/solarisstar/rpgxEF.git
cd ~/rpgxEF || exit -1
apt-get install mesa-common-dev
apt-get install libglu1-mesa-dev
apt-get install libmad0-dev
apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
apt-get install libogg-dev
apt-get install mingw-w64

echo "You should now be setup to run the build scripts for RPG-X. Please tell Telex if you need any assistance"
echo "Please reboot your machine"
