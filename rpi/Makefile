CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lgpiod

TARGET = max7219-rpi
SRCDIR = src
OBJDIR = obj

SRCS = $(SRCDIR)/receiver.c $(SRCDIR)/max7219.c
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean