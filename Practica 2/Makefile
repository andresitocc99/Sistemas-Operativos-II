DIROBJ := bin/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -c -Wall -ansi
LDLIBS := -I$(DIRHEA) -pthread -std=c++11
CC := g++

all : dirs ssoogle 

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

ssoogle: $(DIROBJ)ssoogle.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

palabra: $(DIROBJ)palabra.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test:
	./$(DIREXE)ssoogle "prueba.txt" david 3

test2:
	./$(DIREXE)ssoogle "17-LEYES-DEL-TRABAJO-EN-EQUIPO.txt" equipo 16

test3:
	./$(DIREXE)ssoogle "21-LEYES-DEL-LIDERAZGO.txt" mundial 1000

clean :
	rm -rf *~ core $(DIROBJ) $(DIREXE)
