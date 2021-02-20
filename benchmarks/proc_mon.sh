#!/bin/bash
#set -x
#Usage: proc_mon.sh <Unique Text to Grep> <command to start process>
grepText="${1}"
procStart=$2
logFile=./nya.log

echo "Text to grep to find process: "${grepText}
echo "Command to Start process    : "${procStart}
echo "Log file location           : ${logFile}"

nProc=`ps -ef|grep ${grepText}|grep -v grep|grep -v proc_mon.sh|awk '{print $2}'|wc -l`
echo ${nProc}

case ${nProc}  in
	0)  echo "Starting :     $(date)" >> ${logFile}
		${procStart} &
		;;
	1)  # all ok
		;;
	*)  echo "Removed double proc: $(date)" >> ${logFile}
		kill `ps -ef|grep ${grepText}|grep -v grep|grep -v proc_mon.sh|awk '{print $2}'|head -n 1`
		;;
esac
