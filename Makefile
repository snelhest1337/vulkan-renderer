LIBRARY_DIR = /home/snelhest/libraries

LIBS = tinyobj stb_image
INCS = $(addprefix -I$(LIBRARY_DIR)/,$(patsubst %,%,$(LIBS)))
SRCDIR = src
OBJDIR = obj
TARGETS = main

OBJS = $(addprefix $(OBJDIR)/,$(patsubst %,%.o,$(TARGETS)))

CFLAGS = -std=c++17 -O3 $(INCS)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
APP = renderer
SUBDIRS = shaders 

all: $(SUBDIRS) $(APP)

$(SUBDIRS): 
	$(MAKE) -C $@

$(APP): $(OBJS)
	g++ $(OBJS) -o $(APP) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	g++ $(CFLAGS) $< -c -o $@ $(LDFLAGS)

.PHONY: test clean all $(SUBDIRS) 

test: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
	make -C shaders clean