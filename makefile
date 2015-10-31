CC     = g++
SOURCE = lutg.cpp
EXE    = lutg.exe
OPT    = -std=c++11
GPLOT  = lut.gp

all: lut
	
lut: $(EXE)
	./$(EXE)
	gnuplot $(GPLOT)

$(EXE):
	$(CC) $(OPT) -o $(EXE) $(SOURCE)

clean: 
	rm $(EXE)

clean_out:
	rm *.out *.gp *.dat *.png *.scale 

clean_all: clean clean_out