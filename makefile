objects = main.o
 
a6 : $(objects)
	cc -o a6 $(objects) -lpthread
	   
.PHONY : clean
clean :
	rm a6 $(objects)
	rm expected_output.txt