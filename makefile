all : kolssvin


kolssvin: lab3-5.c GL_utilities.c 
	gcc -Wall -std=c99 -o kolssvin -lGL -lm -lX11 -DGL_GLEXT_PROTOTYPES lab3-5.c GL_utilities.c MicroGlut.c VectorUtils3.c loadobj.c LoadTGA.c

clean :
	rm kolssvin

