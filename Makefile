# Main make file for Twine project
#-Wall -g
#-fpermissive
CPPFLAGS = -pthread -march=native -fmerge-all-constants -g -rdynamic
CXX = g++

FIRST = "first.tw"		#main tester file
SECOND = "second.tw"		#other testing file
TEST = "UnitTest.tw"		#unit tests for testing full functionality of program
BENCHMARK = "Bencmark1.tw"	#testing the speed of parsing and efficiency of ouput code
INTERP_TEST = "Interp_test.tw"
INSTALL =TWINE_INSTALL_BUILD
CLANG =TWINE_USING_CLANG

twine: driver.o
	$(CXX) driver.o -o twine $(CPPFLAGS)

Trans.o: Trans.h Trans.cpp utils/Twine_Utils.h parserNodes.h
	$(CXX) -c Trans.cpp -o Trans.o -std=c++0x $(CPPFLAGS)

Interp.o: Interp.cpp Interp.h includes/__ANY__.h includes/TwineLib.h _interpScope.h #2Parse.o
	$(CXX) -c Interp.cpp -o Interp.o -std=c++17 $(CPPFLAGS)

Parser.o: Parser.cpp Parser.h Token.h Node.h utils/Twine_Utils.h parserNodes.h Operators.h parserNodes.h
	$(CXX) -c Parser.cpp -o Parser.o -std=c++0x $(CPPFLAGS)

2Parser.o: 2Parser.cpp 2Parser.h Token.h utils/Twine_Utils.h parserNodes.h Operators.h parserNodes.h Scope.h Flags.h Linter.h CppParser.h branchParse.cpp resolver.cpp
	$(CXX) -c 2Parser.cpp -o 2Parser.o -std=c++1z $(CPPFLAGS)

CParser.o: CppParser.h 2Parser.h
	$(CXX) -c CppParser.h -o CppParser.o $(CPPFLAGS)

Lexer.o: Lexer.cpp Lexer.h Token.h utils/Twine_Utils.h Operators.h Flags.h
	$(CXX) -c Lexer.cpp -o Lexer.o $(CPPFLAGS)

driver.o: Lexer.o 2Parser.o Trans.o Twine.cpp Flags.h Interp.o Formater.cpp testBench.cpp resolver.cpp
	$(CXX) -c Twine.cpp -o driver.o -std=c++17 $(CPPFLAGS)


#compile headers
compileHeads:
	$(CXX) includes/*.h -w -O3 -s $(CPPFLAGS)
#-Wno-pragma-once-outside-header

#Install
driverInstall: Lexer.o 2Parser.o Trans.o Twine.cpp Flags.h Interp.o Formater.cpp testBench.cpp resolver.cpp compileHeads
	$(CXX) -c Twine.cpp -o driver.o -std=gnu++17 -w -D$(INSTALL) $(CPPFLAGS)

twineInstall: driverInstall
	$(CXX) driver.o -o twine $(CPPFLAGS) -D$(INSTALL) -march=native

#Removes all temp and compiled files
clean:
	find . -type f -name '*~' -delete
	find . -type f -name '*#' -delete
	find . -type f -name 'vgcore.*' -delete
	find . -type f -name '*.o' -delete
	find . -type f -name '*.gch' -delete
	find . -type f -name '*.tw.*' -delete

#Runs program(compile and execute) with defualt file
run:    twine
	./twine $(FIRST)

#Runs interpreter
interp: twine
	./twine -interp -rl 3

#Runs linter on defualt file
lint:   twine
	./twine $(FIRST) -rl 2 -lint -setAll

clang: CXX=clang
clang: twine

#Things for install
install: CPPFLAGS +=-O3 -w 
install: twineInstall

installClang: CXX=clang++
installClang: CPPFLAGS +=-O2 -w -D$(CLANG)
installClang: twineInstall

#cleanCpp: rm -rf *.cpp
#cleanCpp: rm -f *.h

local: install

localClang: installClang

#-------Extra commands for developing-------#
debug: CPPFLAGS += -g -rdynamic -Wall

format: twine
	./twine $(FIRST) -format

#Unit tests
test:   twine
	./twine $(TEST) IGNORE_ALL_WARNING

inttest:  twine
	./twine $(INTERP_TEST_) -rl 2 -interp

#Test speed of comiler/output code
bench:  twine
	./twine $(BENCHMARK) -rl 2

cleaner:
	find . -type f -name '*~' -delete
	find . -type f -name '*#' -delete
	find . -type f -name 'vgcore.*' -delete
	find . -type f -name '*.tw.*' -delete

prog:	twine
	./twine $(FIRST) -rl 2

force:	twine
	./twine $(FIRST) -force

val:	twine
	valgrind --leak-check=yes --track-origins=yes ./twine $(FIRST)

val2:	twine
	valgrind --leak-check=yes --track-origins=yes ./twine $(SECOND)

vali:	twine
	valgrind --leak-check=yes --track-origins=yes ./twine

valti:	twine
	valgrind --leak-check=no --track-origins=no ./twine

#valgrind --leak-check=yes ./twine

other.o: other.cpp
	$(CXX) -c other.cpp -o other.o

other.out: other.o
	$(CXX) other.o -o other $(CPPFLAGS)

other: other.out
	./other
	rm other
	rm other.o

second: twine
	./twine $(SECOND) -force

third: twine
	./twine third.tw
