path=`pwd`
for i in $( ls instancias/ )
do
	file=`echo $i | sed 's/.txt//g'`
	echo $path/code3/main $path/instancias/$i $path/code3/results/$file.ind $path/code3/results/$file.dat
done
