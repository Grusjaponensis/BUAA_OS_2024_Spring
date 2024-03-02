.PHONY: clean

out: calc case_all
	./calc < case_all > out

calc: calc.c
	gcc calc.c -o calc

casegen: casegen.c
	gcc casegen.c -o casegen

case_add: casegen calc
	./casegen "add" 100 | ./calc > case_add

case_sub: casegen calc
	./casegen "sub" 100 | ./calc > case_sub

case_mul: casegen calc
	./casegen "mul" 100 | ./calc > case_mul

case_div: casegen calc
	./casegen "div" 100 | ./calc > case_div

case_all: case_add case_sub case_mul case_div
	touch case_all
	cat case_add > case_all
	cat case_sub >> case_all
	cat case_mul >> case_all
	cat case_div >> case_all

clean:
	rm -f out calc casegen case_* *.o
