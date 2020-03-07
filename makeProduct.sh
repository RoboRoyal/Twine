

#make zip of all requiered files



#remove exist zip
rm -f TwineMaster.zip

#make new dir
mkdir -p src

#extra stuff
sed -i 's/\r//' comm.sh

#copy all files/dirs
cp -f -r -v 2Parser.cpp includes 2Parser.h Interp.cpp parserNodes.h Token.h src/
cp -f -r -v Interp.h Trans.cpp Trans.h branchParse.cpp _interpScope.h resolver.cpp src/
cp -f -r -v Lexer.cpp CppParser.h Lexer.h Scope.h tutorials downloadedImport src/
cp -f -r -v Linter.h Twine.cpp twine.gz Makefile examples UnitTest.tw src/
cp -f -r -v testBench.cpp Flags.h Operators.h Formater.cpp ticTac.tw utils temp1.cpp src/

#cp -f -r -v src/


#zip it
zip -r TwineMaster comm.sh unComm.sh src/*

#remove temp dir
rm -rf src/*


#dirrections for install
#1. Extract/unzip TwineMaster.zip [unzip TwineMaster.zip]
#2. Make comm.sh executable [chmod 777 comm.sh unComm.sh]
#3. Run comm.sh with sudo privlages [sudo ./comm.sh]


if [ 1 -eq 0 ]; then
    unzip TwineMaster.zip
    chmod 777 comm.sh unComm.sh
    sudo ./comm.sh
fi
