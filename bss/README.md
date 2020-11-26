11-24-2020

This is all about initializing BSS to zero
and getting initialized RAM variables set
up right.

We end up with 3 sorts of C variables.

1 - bss variables in ram, set to zero ...

int var1;

2 - data variables in ram, initialized from
 values in flash ...

int var2 = 99;

3 - rodata variables that are in flash ...

const int var3 = 123;

Note also that string constants (such as arguments
to printf) are "const" by default and end up being
handled as "rodata", such as:

puts ( "Hello world\n" );
