LIBRARY_DIR = /home/snelhest/libraries
CC = g++-13
LIBS = tinyobj stb_image
INCS = $(addprefix -I$(LIBRARY_DIR)/,$(patsubst %,%,$(LIBS)))
SRCDIR = src
OBJDIR = obj
TARGETS = main app platform window device swapchain

OBJS = $(addprefix $(OBJDIR)/,$(patsubst %,%.o,$(TARGETS)))

# TODO: add release/debug build modes
CFLAGS = -std=c++20 -O0 -g $(INCS)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
APP = renderer
SUBDIRS = shaders

all: $(SUBDIRS) $(APP)

$(SUBDIRS):
	$(MAKE) -C $@

$(APP): $(OBJS)
	$(CC) $(OBJS) -o $(APP) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -c -o $@ $(LDFLAGS)

$(SRCDIR)/%.cpp: $(SRCDIR)/%.hpp

.PHONY: test clean all $(SUBDIRS)

test: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
	rm -f $(OBJDIR)/*
	make -C shaders clean