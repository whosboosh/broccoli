BROCCOLI_NAME = broccoli

BROCCOLI_OBJECTS = $(shell find src -type f -name '*.cpp')

BROCCOLI_LIBS = -Wl,-rpath -Wl,/usr/local/lib64 -Lsrc/Vendor -lglfw -lvulkan -lshaderc_shared -lassimp -lvulkan -lspirv-cross-core

BROCCOLI_INCLUDE = -Isrc \
				   -I/usr/include \
				   -Ivendor/imgui-1.90 \
				   -Ivendor/GLAD/include \
				   -Ivendor/spirv_cross-1.1.5

EXTRA_CPPFLAGS += "-std=c++17"

VENDOR_OBJECTS = vendor/GLAD/src/glad.c

spirv_cross:
	mkdir -p vendor/spirv_cross-1.1.5/build
	cmake -B vendor/spirv_cross-1.1.5/build -S vendor/spirv_cross-1.1.5 -DSPIRV_CROSS_CLI=OFF DSPIRV_CROSS_SHARED=OFF -DSPIRV_CROSS_STATIC=ON
	make -j8 -C vendor/spirv_cross-1.1.5/build
	mv vendor/spirv_cross-1.1.5/build/libspirv-cross-core.a src/Vendor/

vendor: spirv_cross

.PHONY: vendor

all: vendor
	$(CROSS_COMPILE)$(CXX) $(BROCCOLI_INCLUDE) $(EXTRA_CPPFLAGS) $(BROCCOLI_OBJECTS) $(VENDOR_OBJECTS) -o $(BROCCOLI_NAME) $(BROCCOLI_LIBS)

clean:
	make clean -C vendor/spirv_cross-1.1.5/build