#!/bin/bash

#This is free software; see the source for copying conditions.  There is NO
#warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INSTALL_PATH=/etc/twine


if (( $EUID != 0)); then
    echo "You need to be root to uninstall"
    exit 1
fi

#check if twine command exists
type twine >/dev/null 2>&1 || {

    echo twine doesnt seem to be installed
    
    echo -n "Would you like to continue uninstall anyways?(y/n) "
    read -n 1 ans
    
    if [[ $ans != "y" ]]; then
	echo 
	echo Exiting uninstaller
	exit 1
    fi
    
}
echo 


echo "This will remove the file /bin/twine and the files in /etc/twine/*"
echo -n "are you sure you want to do this?(y/n) "

read -n 1 ans

echo 

if [[ $ans != "y" ]]; then
    echo 
    echo Exiting uninstaller
    exit 1
fi


echo "I am about to run the command rm -rf  $INSTALL_PATH"
echo "If you are uncomfortable with a scrips with sudo privileges running an rm -rf command or if this is not the installed path of twine, please DONT DO THIS"
echo -n "Are you sure you want to proceed?(y/n) "
read -n 1 ans
echo 

if [[ $ans != "y" ]]; then
    echo -n "twine command removed. If you want to do a full removal of all twine files, I would recommend doing an "
    echo -n \"rm -rf\"
    echo " at the installed path default install path is at $INSTALL_PATH"
    echo Exiting uninstaller
    exit 1
fi

echo -n "Are you positive you want to run the command rm -rf $INSTALL_PATH ?"
echo "This will permanently delete all the files at that location. Make sure there is nothing there you want."
echo -n "Continue?(y/n) "
read -n 1 ans
echo 
if [[ $ans != "y" ]]; then
    #big uninstall here
    rm -rf $INSTALL_PATH

    #removing support files here
    rm /bin/twine
    rm /usr/local/share/man/man1/twine.1.gz
    sudo manndb

    if [ -f "comm.sh" ]; then
            rm comm.sh
    fi

    echo "Uninstalled twine"
    echo -n "twine command removed. If you want to do a full removal of all twine files, I would recommend doing an "
    echo -n \"rm -rf\"
    echo " at the installed path. The default install path is at $INSTALL_PATH"
    echo Exiting uninstaller
    exit 0
fi

echo -n "Exiting with uninstalling"
