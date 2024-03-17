# Tries to compile a vertex and fragment shader for each name in shader
SHADERS = triangle
COMPUTE_SHADERS = gradient
COMPILER = glslc


all: $(SHADERS).frag.spv $(SHADERS).vert.spv $(COMPUTE_SHADERS).comp.spv

%.spv: %
	$(COMPILER) $< -o $@

.PHONY: clean all

clean:
	rm *.spv