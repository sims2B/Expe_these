#/bin/bash

for j in '1' '3' '4'
do 
	for i in '1' '4'
	do 
    for k in '10' '5' 
    do ./a.out res_BBv2_noobj_TL7200_h$j\_$i\_$k.txt $k $j $i >> ~/Documents/2017/EJOR/res_BB_fam3.tex
	done
    done
done
echo '\end{document}'>> ~/Documents/2017/EJOR/res_BB_fam3.tex

