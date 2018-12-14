./separator_big5.pl corpus.txt > corpus_seg.txt

/home/student/05/b05902120/srilm-1.5.10/bin/i686-m64/ngram-count -text corpus_seg.txt -write lm.cnt -order 2
/home/student/05/b05902120/srilm-1.5.10/bin/i686-m64/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

for i in {1..10}
do
	./separator_big5.pl testdata/$i.txt > testdata/seg_$i.txt
	/home/student/05/b05902120/srilm-1.5.10/bin/i686-m64/disambig -text testdata/seg_$i.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result1/$i.txt
done
