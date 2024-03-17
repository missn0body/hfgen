CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++23 -g -Ofast
RM = rm

BDIR = bin
SDIR = src
ODIR = obj

BIN = hfgen

SRCS = $(wildcard $(SDIR)/*.cpp)
OBJS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BDIR)/$@

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(BDIR)/* $(ODIR)/*
