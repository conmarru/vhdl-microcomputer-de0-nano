#!/bin/sh

test_dir="$(pwd)/tests/*"
i=0
for file in $test_dir; 
do
	i=$(($i+1))
	./asm $file
	timeout 1s ./emulate-nointeractive.sh output > temp_res
	if [ $? -eq 124 ]; 
	then 
		printf "[ \033[31m!! \033[0m] Test #%d failed\n" $i
	else
		diff temp_res refs/out$i > /dev/null
		[ $? -eq 0 ] && printf "[ \033[32mOK \033[0m] Test #%d succeeded\n" $i
		[ $? -ne 0 ] && printf "[ \033[31m!! \033[0m] Test #%d failed\n" $i
	fi
done

rm temp_res
