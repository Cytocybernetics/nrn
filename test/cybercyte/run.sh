#!/bin/bash

DC1LOG=/home/cytocybernetics/Cybercyte/DC1/SourceFiles/LogFiles/stderr.log
DC1PROG=/home/cytocybernetics/Cybercyte/DC1/Codes/./CCC_DC1
# DC1RAW=~/Cybercyte/DC1/Codes/dc1rawtime.dat # raw file written to cwd.

#sudo echo "hello" # good time to enter root password if needed.
# Hopefully will not need it again for killing DC1PROG

$DC1PROG --no-sudo 2>> $DC1LOG &

nrniv test1.py -

DC1PID=`pidof CCC_DC1`
echo DC1PID=$DC1PID
kill -1 $DC1PID
#MODELSPID=`pidof DC1_Models`
#if [ "$MODELSPID" != "" ]
#then
#    kill -1 $MODELSPID
#fi
