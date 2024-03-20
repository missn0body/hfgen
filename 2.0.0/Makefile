VERSION = 2.0.0

CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++23 -g -Ofast
LIBFLAGS = -ldocopt
RM = rm

LINT = cppcheck
LINTFLAGS = --check-level=exhaustive --enable=all --inconclusive --suppress=missingIncludeSystem --verbose

BDIR = bin
SDIR = src
ODIR = obj

BIN = hfgen

SRCS = $(wildcard $(SDIR)/*.cpp)
OBJS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRCS))

.PHONY: all clean dist self

# BUILDING --------------------------------------------

all: $(BIN)

$(BIN): $(ODIR) $(BDIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BDIR)/$@ $(LIBFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR):
	mkdir $@
$(SDIR):
	mkdir $@
$(ODIR):
	mkdir $@

# RELEASING -------------------------------------------

dist: clean
dist: all
dist: doc/$(BIN).7
	gzip -vk $<
	tar -czvf $(BIN)-$(VERSION).tar.gz $(BDIR)/$(BIN) doc/* ../README.md ../LICENSE --transform 's,^,$(BIN)-$(VERSION)/,'
	md5sum $(BIN)-$(VERSION).tar.gz

self: clean
self: all
self: doc/$(BIN).7
	gzip -vk $<
	sudo mkdir /usr/local/man/man7/ -p
	sudo mv $(BDIR)/$(BIN) /usr/local/bin/
	sudo mv doc/$(BIN).7.gz /usr/local/man/man7/

# OTHER -----------------------------------------------

lint: $(SRCS)
	$(LINT) $(LINTFLAGS) $<

clean:
	$(RM) -f $(BDIR)/* $(ODIR)/*
