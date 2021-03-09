
# Setting up your environment

Disclamer: these steps work on Linux. Windows and OSX is not supported.
You will g++ or clang which can compile C++11 or later. g++ is recomended. 
You will need sudo privliges to install the twine command, but won't need it if you are just using it locally.

1. Open a terminal and navigate to where you want the examples and tutorials folders to go.

2. Run the command `git clone https://github.com/TWINE_REPOSITORY `. This will download the Twine source code //TODO

3. Move into the new directory


If you don't want to install twine and would rather only make it locally, skip to step 8.


4. Run the installer with 'sudo ./comm.sh'

5. Follow the instructions in the installer to complete the installation. 

6. Run it with the command `twine examples/helloWorld.tw` to ensure that twine is installed and working correctly.

7. If it works, you can now move on to the "basic concepts" tutorial.

----------------Only do if you skipped steps 4-7-----------------

8. Move to the src directory.

8. Run './make local'

10. Run the command './twine ../examples/helloWorld.tw` to ensure that twine is installed and working correctly.

11. If it works, you can now move on to the "basic concepts" tutorial.


[index](index.md) | [next: Basic Concepts ->](1_basic_concepts.md)


