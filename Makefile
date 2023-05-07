CC = g++
CFLAGS = -Wall -c
LFLAGS = -Wall
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(BINDIR)/program

$(BINDIR)/program: $(OBJ)
    $(CC) $(LFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
    $(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

clean:
    rm -f $(OBJDIR)/*.o $(BINDIR)/program
