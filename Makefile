.PHONY: all clean

all: testa

testa: testa

clean:
	-rm testa 

install:
	cp testa /usr/local/bin
	