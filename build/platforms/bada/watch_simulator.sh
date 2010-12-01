#!/bin/sh

exit_marker="BADA APPLICATION EXITED. RETURN CODE IS:"

IFS=""

while true
do
  read line

  if [ "$line" == "$exit_marker" ]
  then
    read code
    exit `expr $code`
  fi

  if ! [ -z "$line" ] 
  then
    echo $line
  fi
done
