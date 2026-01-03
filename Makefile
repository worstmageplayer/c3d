CC := gcc
CFLAGS := -Wall -Wextra -O2 -I./raylib-5.5_linux_amd64/include -MMD -MP
LDFLAGS := -L./raylib-5.5_linux_amd64/lib -Wl,-rpath,'$$ORIGIN/raylib-5.5_linux_amd64/lib'
LIBS := -lraylib -lm

SRCDIR := src
OBJDIR := obj

SRCS := $(shell find $(SRCDIR) -name '*.c')
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

DEPS := $(OBJS:.o=.d)

TARGET := main

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@ $(LIBS)

# compile .c -> obj/.o, create obj dir if needed (order-only prerequisite)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ensure obj dir exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# include auto-generated dependency files (if present)
-include $(DEPS)

clean:
	@rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -O0
debug: clean all
