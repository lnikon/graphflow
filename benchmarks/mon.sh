#!/bin/bash

my_log="ftp_$(date +%Y%m%d%H%M%S).log"

my_pid=$(pgrep hellwrls)
echo "my_pid=${my_pid}"
sleep 15
still_there=1
while [[ $still_there -gt 0 ]]
do
	if [[ $(ps ax | grep $my_pid | grep -v grep) ]] # If your PID is still in the process table
	then
		# Insert monitoring stuff in here that > to $my_log
		still_there=1
	else
		still_there=0
	fi
	sleep 1
done
