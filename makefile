all:
	gcc *.c *.h -o test_uart -lpthread 
clean:
	rm -rf *.o test_uart