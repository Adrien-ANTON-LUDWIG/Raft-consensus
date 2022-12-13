# Raft consensus algorithm

### Authors:
 - ANTON-LUDWIG Adrien
 - GIDEL Melvin
 - INDJEIN Nicolas
 - LORRAIN Nicolas
 - PLUQUET Adèle

## Usage

Build : 
```sh
make
```

Execution :

```
./run.sh [flags]

flags:
-c, --client_n  Set client count. Default to 2.
-s, --server_n  Set server count. Default to 3.
--commands      Set the command file to execute. Default to none.
-l, --show_logs Show application logs after exiting.
--cli           Force CLI mode for REPL
```

## REPL CLI Usage

For commands CRASH and START, you have to specify the rank of the process you want to alter.

##### CRASH 
```sh
Usage: crash <rank>
```
```sh
REPL CLI enabled
Server ranks: [0, 2] | Client ranks: [3, 4]
> crash 0
>
```
##### START
```sh
Usage: start <rank>
```
```sh
REPL CLI enabled
Server ranks: [0, 2] | Client ranks: [3, 4]
> start 3
>
```
\
For command SPEED, you also need to specify the desired speed between *"low"*, *"medium"* and *"high"*
##### SPEED 
```sh
Usage: speed <rank> low|medium|high
```
```sh
REPL CLI enabled
Server ranks: [0, 2] | Client ranks: [3, 4]
> speed 2 low
>
```
\
We also added an INFO command that only requires the rank of the process. It yields the current values of the specified process.
##### INFO
```sh
Usage: info <rank>
```
```sh
REPL CLI enabled
Server ranks: [0, 2] | Client ranks: [3, 4]
> info 0
Server (rank 0) infos:
- Speed: HIGH
- Is crashed: NO
- Is leader: NO
> 
```

## Command List Usage

For the command list usable by clients, you have to use a command file. You have to specify it with the flag ```--commands```.
In this file, you can write your commands like below:

```--```: Sets the next client to listen to the following commands.\
```<CMD> <args>```: Send the command ```CMD``` to the servers with the right ```args```.\
```$```: Specifies the end of commands. On the next lines, you can add REPL commands.\
```@<time>```: Sleeps for ```time``` milliseconds. Can only be used while in the REPL commands part.

Example file :
```txt
--
LOAD file1.txt
LOAD file2.txt
--
LOAD file3.txt
--
LOAD file4.txt
$
start 3
start 4
start 5
@6000
exit
```

This file will have clients 0, 1 and 2 (with resp. ranks 3, 4 and 5 because we assume there are two servers), to load the following files. Were you to have more clients, they would not do anything, even if started.
If client 1 (rank 4) were not started, client 2 (rank 5) would still try to load "file4.txt" and not try to do the work of client 1.

## Running the tests

Just run `./test_runner.sh` and wait.

Be sure to have build the application before.

## Built With

[![C++](https://img.shields.io/badge/-C%2B%2B-red?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.wikipedia.org/wiki/C%2B%2B)
[![Open MPI](https://img.shields.io/badge/Open%20MPI-006fb0?style=for-the-badge)](https://en.wikipedia.org/wiki/Open_MPI)
[![Spdlog](https://img.shields.io/badge/-Spdlog-333333?style=for-the-badge&logo=Spdlog&logoColor=white)](https://github.com/gabime/spdlog)
[![JSON for Modern C++](https://img.shields.io/badge/JSON-333333?style=for-the-badge)](https://json.nlohmann.me/)
[![Makefile](https://img.shields.io/badge/Makefile-333333?style=for-the-badge)](https://en.wikipedia.org/wiki/Make_(software)#Makefile)
[![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=CMake&logoColor=white)](https://en.wikipedia.org/wiki/CMake) 
[![Bash](https://img.shields.io/badge/Bash-4EAA25?logo=GNU%20Bash&logoColor=white&style=for-the-badge)](https://en.wikipedia.org/wiki/Bash_(Unix_shell))