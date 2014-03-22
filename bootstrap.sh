#/bin/bash

#list all the command of servers to startup
log_file='/var/log/epstat.log'

declare -a svr_list
svr_list[0]='/var/www/stat_server/udpserver/epstat 9300 192.168.1.217 6379' 
svr_list[1]='/var/www/stat_server/udpserver/epstat 9301 192.168.1.217 6379' 

#process server command
boot_svr () {
for cmd in "${svr_list[@]}";
do
	#check if the server process exists
	pcount=`ps aux | grep "$cmd" | grep -v 'grep' | wc -l` 
	if [ $pcount -gt 0 ]
	then
		continue
	fi	
	
	echo "<<======================" >> $log_file	
	#exec commmand to start up server
	full_command_str=$cmd' >> '$log_file' 2>&1 &'
	echo `date +%F_%T`" "$full_command_str >> $log_file
	echo $full_command_str | sh
done
}

r_times=0;
while [ $r_times -lt 20 ]
do
	((r_times++))
	boot_svr	
	sleep 5
done
