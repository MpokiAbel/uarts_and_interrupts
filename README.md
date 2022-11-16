# GIT BRANCHES 
There are three branches i worked on which are: master,week1 and week2.
master branch contains the most recent work which is to be marked, week2
contains the work for week 2 and week1 contains work only for the first
part regarding the console.

# How to compile
Enter "make run" to run the program in the arm.boot directory and 
if you are interested in debugging enter "make debug" then open a 
new terminnal and enter the following:

arm-none-eabi-gdb kernel.elf
(gdb)target remote localhost:1234

# What works
All the mandatory sections of the project were implemented hence
the shell works and so does the interupts.

# What does not work
The optional part on adding event-oriented programming model
was not implemented on this work