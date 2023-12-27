# HW1- A Simple Shell

## Overview of this assignment

Control flow of your simple shell:
1. Display the prompt sign “>” and take a string from user
2. Parse the string into a program name and arguments
3. Fork a child process
4. Have the child process execute the program
5. Wait until the child terminates
6. Go to the first step

## Important System Calls
fork()
exec() family
wait() family

## Waiting on child processes
If a command is ended with “&”, then the shell will not wait on a child process \
For example:
> sleep 10s \
> The prompt re-appears after 10 seconds \
> sleep 10s & \
> The prompt re-appears immediately \

## Test Cases
Your shell must correctly handle test cases of the following format
```<program> <arg1> <arg2> <...> ```

Test cases are like the following, but not limited to:
```bash
clear
ls -l
cp a.txt b.txt
cat c.txt &
```

Do not leave zombie processes in the system (Before or after your shell terminate) 

## Bonus
I/O redirection 
```bash
ls -l > a.txt
```

Pipe
```bash
ls | more
```

No multiple or combined pipe/redirections
```bash
a | b | c
a < b > c
a | b > c
```

## Note
Don't use system() and popen() to do pipe or I/O-redirection
