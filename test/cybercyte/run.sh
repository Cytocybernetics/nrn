#!/bin/bash

DC1LOG=/home/cytocybernetics/Cybercyte/DC1/SourceFiles/LogFiles/stderr.log
DC1PROG=/home/cytocybernetics/Cybercyte/DC1/Codes/./CCC_DC1
# DC1RAW=~/Cybercyte/DC1/Codes/dc1rawtime.dat # raw file written to cwd.

sudo echo "hello" # good time to enter root password if needed.

sudo taskset -c 6 $DC1PROG 2>> $DC1LOG
