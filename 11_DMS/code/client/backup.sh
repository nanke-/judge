#!/bin/bash
# ls -l
# ps
# cal #看日历
# >& 将出错信息重定向到/dev/null
if cp $1 $2 >& /dev/null
then
	if cat /dev/null > $1
	then
		exit 0
	else
		exit 2
	fi
else
	exit 1
fi
