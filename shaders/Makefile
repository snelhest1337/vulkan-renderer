# Tries to compile a vertex and fragment shader for each name in shader
SHADERS = triangle
COMPILER = glslc


all: $(SHADERS).frag.spv $(SHADERS).vert.spv

%.spv: %
	$(COMPILER) $< -o $@

.PHONY: clean all

clean: 
	rm *.spv