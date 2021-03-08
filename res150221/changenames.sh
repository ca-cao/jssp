while read p 
do a=`echo "$p" |awk '{print $2}'` 
   b=`echo "$p" |awk '{print $1}'` 
   mv $a $b
done <fnames
