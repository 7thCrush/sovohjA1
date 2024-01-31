# Assignment 1:

## How to operate the assignment via make-commands:

- `make` creates the object files needed to run the assignment
- `make test` tests the assignment via testbench
- `make clean` cleans the object files

## Notes:

Remember to execute the make-commands in the same directory which has the sockets, and `make launch` and `make test` in different terminals.
If you want to repeat a particular random input, you can
specify the seed of that run with command line argument "-s <num>".

If you give "-l" argument, very long test is also run with 1MB input size.
