CC     = gcc
CFLAGS = -Wall -std=c99 -Iheader
LFLAGS = -lm
 
all:
	mkdir -p objects output
	$(CC) $(CFLAGS) -c c_files/vector.c    -o objects/vector.o
	$(CC) $(CFLAGS) -c c_files/trajectory.c -o objects/trajectory.o
	$(CC) $(CFLAGS) -c c_files/planet.c    -o objects/planet.o
	$(CC) $(CFLAGS) -c c_files/solvers.c   -o objects/solvers.o
	$(CC) $(CFLAGS) -c c_files/energy.c    -o objects/energy.o
	$(CC) $(CFLAGS) -c c_files/export.c    -o objects/export.o
	$(CC) $(CFLAGS) -c c_files/main.c              -o objects/main.o
	$(CC) objects/*.o -o kepler $(LFLAGS)
 
clean:
	rm -rf objects kepler