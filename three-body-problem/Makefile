CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama -lXext

all: tbp
tbp: three-body-problem.c
	$(CC) $(CFLAGS) three-body-problem.c -o tbp $(LIBS)
	
clean:
	rm -rf tbp