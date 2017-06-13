INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
OUTDIR = ./bin

SRCS = $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c)

CC = gcc
CCFLAGS = -I$(INCDIR) -Wall -Wextra -Wpedantic -ansi -O3

AR = ar
ARFLAGS = rvs

all: libcgoodies.so libcgoodies.a

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CCFLAGS)

$(OBJDIR)/%.po: $(SRCDIR)/%.c
	$(CC) -fPIC -c -o $@ $< $(CCFLAGS)

OBJS := $(patsubst %.c, %.o, $(SRCS))
OBJS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(OBJS))

POBJS := $(patsubst %.c, %.po, $(SRCS))
POBJS := $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(POBJS))

libcgoodies.so: $(POBJS)
	$(CC) -shared -Wl,-soname,$@ -o $(OUTDIR)/$@ $^ $(CCFLAGS)

libcgoodies.a: $(OBJS)
	$(AR) $(ARFLAGS) $(OUTDIR)/$@ $^

.PHONY: clean

clean:
	rm -f $(OUTDIR)/* $(OBJDIR)/*.o $(OBJDIR)/*.po \
		$(OBJDIR)/**/*.o $(OBJDIR)/**/*.po
