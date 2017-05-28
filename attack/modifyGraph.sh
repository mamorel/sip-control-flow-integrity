#!/bin/bash

usage () {
	printf "Usage: $0 <binary file> <path to new graph.txt> <path to old graph.txt>\n"
	printf "Example: '$0 ./something newGraph.txt graph.txt'\n"
	exit 1
}

if [ $# -ne 3 ]
then
	printf "Wrong number of parameters\n"
	usage
fi

oldDigest=`openssl dgst -sha256 $3`
IFS=' ' read -r -a array <<< "$oldDigest"
echo ${array[1]}

mv "$2" "$3" 

newDigest=`openssl dgst -sha256 $3`
IFS=' ' read -r -a array2 <<< "$newDigest"
echo ${array2[1]}

perl -pi -e "s/${array[1]}/${array2[1]}/g" $1
