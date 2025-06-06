#!/bin/bash
#
# $1 = Direction of FXP "from" or "to"
# $2 = IPv4
# $3 = Name of the file
# $4 = Actual path the file is stored in
#
# EXIT codes..
# 0 - Good
# 2 - Bad
# 1 - Ugly :-)
#
# Example script for pre_fxp_check

UNWANTED_LIST="127.0.0.1"
if [[ -f "$4/$3" ]]; then
	for UNWANTED in $UNWANTED_LIST; do
		if [[ $UNWANTED == $2 ]]
		then
			echo "fxp denied."
			exit 2;
		fi
	done;
fi
