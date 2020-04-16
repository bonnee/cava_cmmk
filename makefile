IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =/usr/lib

LIBS=-lcmmk -lusb -lm

_DEPS = keeb.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cava_cmmk.o keeb.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cava_cmmk: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

install: cava_cmmk
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 cava_cmmk $(DESTDIR)$(PREFIX)/bin/

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 