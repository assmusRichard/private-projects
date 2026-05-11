CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama -lXext

all: colliding-circles

colliding-circles: colliding-circles.c
	$(CC) $(CFLAGS) colliding-circles.c -o colliding-circles $(LIBS)

clean:
	rm -rf colliding-circles	