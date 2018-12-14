make
for i in {1..10}
do
	./my_disambig -text testdata/seg_$i.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result2/$i.txt
done
