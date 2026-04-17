This is a "quick and dirty" MIPS32r1 software implementation that I made for
a singular purpose: Compiling real C code to a virtual machine. I had practiced
making a few emulators before of my own toy architecture designs, but never
anything I could compile real code to. Having accomplished my initial goal 
pretty quickly, I never fully finished this project. I've moved on to new
horizons. 

The bulk of the ISA implementation are in the header files in include/private;
instruction_headers.h and mips32r1_isa.h in particular.
