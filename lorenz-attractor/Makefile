CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama -lXext

all: la
la: lorenz-attractor.c
	$(CC) $(CFLAGS) lorenz-attractor.c -o la $(LIBS)
clean:
	rm -rf la