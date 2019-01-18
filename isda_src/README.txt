/*
 * ISDA CDS Standard Model
 *
 * Copyright (C) 2009 International Swaps and Derivatives Association, Inc.
 * Developed and supported in collaboration with Markit
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the ISDA CDS Standard Model Public License.
 */


Package contents
----------------

The 'examples' directory contains an example Excel workbook demonstrating the library, 
and a simple program showing how to
call it from 'C'.

The 'excel' directory contains code for the Excel addin.

The 'lib' directory contains the actual analytics code.


Building
--------

To build the library go to the 'build' directory within the target's directory, 
then to the directory for the desired platform.
Typing make will invoke the build.

Note that for the library itself there is another level within the build directory 
depending on whether a static or dynamic
library is to be built.

Within the tests directory 'make retest' will cleanup after a previous test run, 
and 'make tests' will rerun all of the
regression tests.

##########################
Example: FOR MAC IOS
###########################
Go to folder "isda_cds_model_c_v1.8.2/lib/build/dll/linux"
Run command: "make"
To run the example: go to folder
"isda_cds_model_c_v1.8.2/examples/c/build/linux"
Run command: "./cds"

#################################################
The make command compiles using following lines
#################################################

usr/bin/gcc -o main.o -g -c -DUNIX -DLINUX -DVERSION="1.8.2" -I../../../../lib/include/isda -I/usr/include "../../../../examples/c/src/main.c"
/usr/bin/g++ -lm -o cds main.o ../../../../lib/build/lib/linux/cds.a  -lc
chmod u+x cds
