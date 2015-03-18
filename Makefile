OBJETOS=Diff.o LCS.o Imprimir.o maindiff.o Error.o
CFLAGS=-lcrypto -g

.SILENT: 

all: my-diff clean
	make -C repo

my-diff: $(OBJETOS)
	gcc -o my-diff $(OBJETOS) $(CFLAGS)

%.o: %.c 
	gcc -c -o $@ $< -g

install: 
	cp ./my-diff /bin/
	
clean: 
	rm -f $(OBJETOS)
