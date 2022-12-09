#!/bin/sh

test_dir="$(pwd)/tests/*"
i=0
for file in $test_dir; 
do
	i=$(($i+1))
	./asm $file
	./emulate-nointeractive.sh output > temp_res
	diff temp_res refs/out$i > /dev/null
	[ $? -eq 0 ] && printf "[ \033[32mOK \033[0m] Test #%d succeeded\n" $i
	[ $? -ne 0 ] && printf "[ \033[31m!! \033[0m] Test #%d failed\n" $i
done

rm temp_res
