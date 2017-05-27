printf "Running 50 times\n"

baseName="input_"

for j in {0..6}
do
	printf "For functions: '%s'" "$(cat $baseName$j.txt | tr '\n' ' ')"
	time for i in {1..50}; do ./compile.sh ../memoryTrace3.c ../memoryTrace3_$j input_$j.txt ; done
	mv graph.txt graph_$j.txt
	mv ../code/NewStackAnalysis.c ../code/NewStackAnalysis_$j.c
done
