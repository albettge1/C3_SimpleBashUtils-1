s21_grep:
	gcc -Wall -Werror -Wextra s21_grep.c -o s21_grep

clang: 
	clang-format --style=Google -i s21_grep.c

clean:
	rm -rf s21_grep
	
rebuild:
	make clean
	make
test:
	sh test_func_grep.sh

leaktest:
	sh test_leak_grep.sh
