OBJDIR := obj
BINDIR := bin
SRCDIR := src

TARGET_NAME := cava_cmmk
TARGET := $(BINDIR)/$(TARGET_NAME)

CC := gcc
CFLAGS := -I$(SRCDIR)

LDIR := /usr/lib
LIBS := -lcmmk -lusb -lm -lhidapi-libusb

_DEPS := keeb.h
DEPS := $(patsubst %,$(SRCDIR)/%,$(_DEPS))

_OBJ := cava_cmmk.o keeb.o 
OBJ := $(patsubst %,$(OBJDIR)/%,$(_OBJ))

ifeq ($(PREFIX),)
    PREFIX := /usr
endif

default: all

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(BINDIR)/cava_cmmk $(DESTDIR)$(PREFIX)/bin/

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@echo CLEAN $(TARGET) $(OBJ)
	@rm -f $(TARGET) $(OBJ)
	@echo REMOVE $(BINDIR) $(OBJDIR)
	@rm -d $(BINDIR) $(OBJDIR)