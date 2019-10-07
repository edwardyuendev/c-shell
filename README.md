# The Goals of this Project were:
1. develop a simple shell application
2. understand important Unix system calls
3. improve proficiency with C language

# Shell Description
The application is intended as a shell (command line interpreter) that accepts input from the user and executes the commands that are given. It will be most similar to the well-known shell *bash*.

The shell is able to execute commands, redirect standard input or standard output to files, pipe output of commands to other commands, and executes commands in the background. It can run most commands that *bash* can run and can process the metacharacters: '<', '>', '|', and '&'.

The structure of the program is essentially:
1. Parses the input entered through the Terminal or Command Line
2. Based on parsed input, different booleans are set causing different fork(), exec(), and dup() calls where necessary

# How To Run
1. Have the Makefile and simple_shell.c file in the same directory
2. Type "make"
3. Type "./simple_shell" in order to run


# Restrictions: 
- Only one '&' character can appear and must be the last on the command line
- Only the **first** command can have its input redirected and only the **last** can have its output redirected.
- In the case of a single command, you can apply both the input and output redirection to the command ( cat < x > y )
- Wrote code to specifically work on Linux (Fedora OS), but works on Mac OSX as well. Not tested on Windows.
