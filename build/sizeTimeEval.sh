usage () {
	printf "Usage: $0 <origBinary> <binaryName> <runs>\n"
	printf "\t<origBinary> is the name of the original binary\n"
	printf "\t<binaryName> is the name of the binary.\n"
	printf "\t<runs> is the number of times to run.\n"
	exit 1
}

if [ $# -ne 3 ]
then
	usage
fi

origBinary=$1
binary=$2
runs=$3

printf "Orig: '%s' , New: '%s', runs: '%s'" "$origBinary" "$binary" "$runs"

origBinSize=$(stat --printf="%s" ../$origBinary)
printf "Orig binary size: %s\n" "$origBinSize"

t="$(time ( for((i=0; i<runs;i++)); do ../$origBinary 100; done ) 2>&1 1>/dev/null )"

printf "\tOriginal time: %s\n""$t"

for connectivity in {1..6}
do
	printf "\nRunning '%s' %s times\n" "$binary$connectivity" "$runs"

	mv graph_$j.txt graph.txt
	t="$(time ( for((i=0; i<runs;i++)); do ../$binary$connectivity 100; done ) 2>&1 1>/dev/null )"

	printf "\tTime for connectivity %s: %s\n" "$connectivity" "$t"

	mv graph.txt graph_$j.txt
	size=$(stat --printf="%s" ../$binary$connectivity)
	printf "\tBinary size for connectivity %s: %s\n" "$connectivity" "$size"
done
