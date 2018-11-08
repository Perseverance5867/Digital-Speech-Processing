for i in {1..50}
do
	iter=$((100*i))

	./train $iter model_init.txt seq_model_01.txt model_01_$i.txt &
	./train $iter model_init.txt seq_model_02.txt model_02_$i.txt &
	./train $iter model_init.txt seq_model_03.txt model_03_$i.txt &
	./train $iter model_init.txt seq_model_04.txt model_04_$i.txt &
	./train $iter model_init.txt seq_model_05.txt model_05_$i.txt &

	echo model_01_$i.txt > modellist_$i.txt
	echo model_02_$i.txt > modellist_$i.txt
	echo model_03_$i.txt > modellist_$i.txt
	echo model_04_$i.txt > modellist_$i.txt
	echo model_05_$i.txt > modellist_$i.txt
	
	# ./test modellist_$i.txt testing_data1.txt result1_$i.txt
	# ./acc testing_answer.txt result1_$i.txt acc1_$i.txt

	# echo `cat acc1_$i.txt` $i >> cmp.txt
done
