OBJETOS=../Diff.o ../LCS.o ../Error.o ../Imprimir.o Content.o File.o Hash.o mainrepo.o Messages.o my-repo.o Path.o SList.o Tree.o
CFLAGS=-lcrypto -g

.SILENT: 

all: my-repo clean

my-repo: $(OBJETOS)
	gcc -o my-repo $(OBJETOS) $(CFLAGS)

%.o: %.c
	gcc -c -o $@ $< -g

install:
	cp ./my-repo /bin/

clean:
	rm -f $(OBJETOS)
