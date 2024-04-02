ROOTDIR = .
OBJDIR = $(ROOTDIR)/obj


# RNG generators
GEN_OBJ = $(OBJDIR)/main.o $(OBJDIR)/bitarray.o $(OBJDIR)/generators_r.o $(OBJDIR)/fn.o $(OBJDIR)/ConfigStructure.o 
GEN_OBJ_D = $(OBJDIR)/d_main.o $(OBJDIR)/d_bitarray.o $(OBJDIR)/d_generators_r.o $(OBJDIR)/d_fn.o $(OBJDIR)/d_ConfigStructure.o 

generator_main: $(GEN_OBJ) time_measure.h
	gcc -O3 $(GEN_OBJ) -o $@
$(OBJDIR)/main.o: main.c
	gcc -O3 -c main.c -o $@
$(OBJDIR)/bitarray.o: bitarray.h bitarray.c
	gcc -O3 -c bitarray.c -o $@
$(OBJDIR)/generators_r.o: generators.h generators.c
	gcc -O3 -c generators.c -o $@
$(OBJDIR)/fn.o: fn.h fn.c
	gcc -O3 -c fn.c -o $@
$(OBJDIR)/ConfigStructure.o: ConfigStructure.h ConfigStructure.c
	gcc -O3 -c ConfigStructure.c -o $@

main_debug: $(GEN_OBJ_D) time_measure.h
	gcc -g $(GEN_OBJ_D) -o main.exe
$(OBJDIR)/d_main.o: main.c
	gcc -g -c main.c -o $@
$(OBJDIR)/d_bitarray.o: bitarray.h bitarray.c
	gcc -g -c bitarray.c -o $@
$(OBJDIR)/d_generators_r.o: generators.h generators.c
	gcc -g -c generators.c -o $@
$(OBJDIR)/d_fn.o: fn.h fn.c
	gcc -g -c fn.c -o $@
$(OBJDIR)/d_ConfigStructure.o: ConfigStructure.h ConfigStructure.c
	gcc -g -c ConfigStructure.c -o $@

clean:
	rm *.exe
	rm $(OBJDIR)/*.o