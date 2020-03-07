#!/bin/bash

#This is free software; see the source for copying conditions.  There is NO
#warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INSTALL_PATH=/etc/twine
USING_GCC=true

#TODO
#INSTALLER=apt-get install

if (( $EUID != 0)); then
    echo "You need to be root to install Twine"
    echo "Alternitivly you can use run make local or make localClang to only install twine in this director"
    echo "Then use ./twine to run"
    exit 1
fi

#check if twine command exists
type twine >/dev/null 2>&1 && {

    echo "Twine seems to already be installed"

    echo -n "Would you like to continue install anyways?(y/n) "
    read -n 1 ans
    
    if [[ $ans != "y" ]]; then
	echo 
	echo Exiting installer
	exit 2
    fi
    
}
echo 

#echo "Twine can be install using clang++ or g++"
#echo "The defualt is g++"
echo "This install may mess with other files in this or sub directories. Please make sure they are clear"

#check for g++
if hash g++ 2>/dev/null; then
    echo "Using g++ for compiling/execution"
	#using g++
	#do nothing, its the defualt
elif hash clang 2>/dev/null; then
	#using clang
	USING_GCC=false
else
    echo "A C++ compiler is neccesary for installation"
    echo -n "Would you like to install g++ now?(y/n) "
    read -n 1 ans
    echo
    if [[ $ans == "y" ]]; then
	echo "apt-get install g++"
        apt-get install g++
    else
       echo "Twine currently only works with g++ and clang. Please install one of these and try installing again"
       echo "Exiting installer"
       exit 3
    fi

fi

#----move extra stuff----

mv src/tutorials .
mv src/examples .

#----Add man file------
if [ -f "src/twine.gz" ]; then
    mv src/twine.gz /usr/share/man/man1/twine.1.gz
    sudo mandb
fi

#check for wget
type wgeta >/dev/null 2>&1 || {
    echo "wget is neccesary for including files from the web"
    echo "All other features will work without it, and you can instal it later at any time"
    echo -n "Would you like to install wget now?(y/n) "
    read -n 1 ans
    echo 
    if [[ $ans == "y" ]]; then
	echo "apt-get install wget"
	apt-get install wget
    fi
}

DEFUALT_INSTALL="YES"

#ask for prefered installed path
echo -n "Defualt install path of twine is at "
echo $INSTALL_PATH
echo -n "Would you like to install using the default path?(y/n) "
read -n 1 ans
echo 
if [[ $ans != "y" ]]; then
    echo -n "Enter the absolute path you would like to install twine at (it should start with an '/'): "
    read INSTALL_PATH
    DEFUALT_INSTALL="NO"
fi

echo "Installing at: $INSTALL_PATH"

#move files to install path
if [ -d "src" ]; then
	mkdir $INSTALL_PATH
	mv src/ $INSTALL_PATH
else
	echo "You seem to be missing the src directory...."
	echo "This means I cant install twine..."
	echo "This is awk..."
	echo "Sorry?"
	echo "Im just going, to, um go..."
	exit 99
fi

#-------make twine command----------
touch /bin/twine

if [[ "$DEFUALT_INSTALL" == "YES" ]]; then
    echo "#!/bin/bash

#luancher for twine

$INSTALL_PATH/src/twine \$@" > /bin/twine
else
    echo "#!/bin/bash

#luancher for twine
$INSTALL_PATH/src/twine -INSTALL_PATH $INSTALL_PATH/src/ \$@" > /bin/twine
fi

#allow read and execute permission on files
chmod 555 /bin/twine
chmod -R 555 "$INSTALL_PATH/src/"

#-----compile Twine------
#cd $INSTALL_PATH/src
#echo Moving to $INSTALL_PATH/src
if [ "$USING_GCC" == false ];then
    make clean -s
    make installClang -C "$INSTALL_PATH/src"
else
    make clean -s
    make install -C "$INSTALL_PATH/src"
fi
#echo Back to $CURRENT_PATH
#cd $CURRENT_PATH
#---------------------


#-----remove un-needed files------
echo -n "Would you like to remove source files? They are no longer needed(y/n) "
read -n 1 ans
echo
if [[ $ans == "y" ]]; then
    #remove source files
    find . -type f -name '*.h' -delete
    find . -type f -name '*.cpp' -delete
    rm -f TwineMaster.zip
    
    #remove testing files
    find . -type f -name '*.tw' -delete
fi

#remove other files
find . -type f -name '*.txt' -delete

if [ -f ".gitignore" ]; then
        rm .gitignore
fi
#-------------------------------

#-----test if install was successful--------
if [ type twine >/dev/null 2>&1 ]; then
	echo "Twine has been installed. If you wish to uninstall twine at any time, simply run sudo ./unComm.sh"
	echo "Tutorials and examples can be found in their respective directories"
	echo "You can also check out 'man twine' and 'twine -help' for help with running twine"
	echo "Thanks for trying out twine, all feedback is greatly appreciated"
else
	echo "An unknow error has occured"
fi
#------------------------------------------

#-0c
