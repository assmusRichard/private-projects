CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama -lXext

all: euler rk4
euler: double-pendulum-euler.c
	$(CC) $(CFLAGS) double-pendulum-euler.c -o euler $(LIBS)

rk4: double-pendulum-rk4.c 
	$(CC) $(CFLAGS) double-pendulum-rk4.c -o rk4 $(LIBS)

clean:
	rm -rf euler	
	rm -rf rk4