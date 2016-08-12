# Set the following PATHs before compiling COLi
ISL_INCLUDE_DIRECTORY=/Users/b/Documents/src/MIT/IR/isl_jan_2016_prefix/include/
ISL_LIB_DIRECTORY=/Users/b/Documents/src/MIT/IR/isl_jan_2016_prefix/lib/
HALIDE_SOURCE_DIRECTORY=/Users/b/Documents/src/MIT/halide/halide_src/
HALIDE_LIB_DIRECTORY=/Users/b/Documents/src/MIT/halide/halide_src/bin/


# Examples
#ISL_INCLUDE_DIRECTORY=/Users/b/Documents/src/MIT/IR/isl_jan_2016_prefix/include/
#ISL_LIB_DIRECTORY=/Users/b/Documents/src/MIT/IR/isl_jan_2016_prefix/lib/
#HALIDE_SOURCE_DIRECTORY=/Users/b/Documents/src/MIT/halide/halide_src/
#HALIDE_LIB_DIRECTORY=/Users/b/Documents/src/MIT/halide/halide_src/bin/


EXTRA_FLAGS=-O3
HALIDE_LIB_FLAGS=-lHalide  `libpng-config --cflags --ldflags`


all: compile


compile:
	mkdir -p build
	g++ -g -c -std=c++11 ${EXTRA_FLAGS} src/coli_debug.cpp -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_debug.o
	g++ -g -c -std=c++11 ${EXTRA_FLAGS} src/coli_codegen_c.cpp -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_codegen_c.o
	g++ -g -c -std=c++11 ${EXTRA_FLAGS} src/coli_codegen_halide.cpp -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_codegen_halide.o
	g++ -g -c -std=c++11 ${EXTRA_FLAGS} src/coli_computation.cpp -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_computation.o
	g++ -g -c -std=c++11 ${EXTRA_FLAGS} src/coli_ir.cpp -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_ir.o


test:
	g++ -g -std=c++11 ${EXTRA_FLAGS} tests/test_isl.cpp build/coli_debug.o build/coli_ir.o build/coli_computation.o build/coli_codegen_c.o build/coli_codegen_halide.o -L${ISL_LIB_DIRECTORY} -lisl -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -L${HALIDE_LIB_DIRECTORY} ${HALIDE_LIB_FLAGS} -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/coli_test_executable
	@echo; echo;
	@DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:${HALIDE_LIB_DIRECTORY} build/coli_test_executable
	g++ -g -std=c++11 ${EXTRA_FLAGS} tests/generated_code_wrapper.cpp LLVM_generated_code.o -L${ISL_LIB_DIRECTORY} -lisl -I${HALIDE_SOURCE_DIRECTORY}/include -I${HALIDE_SOURCE_DIRECTORY}/tools -L${HALIDE_LIB_DIRECTORY} ${HALIDE_LIB_FLAGS} -Iinclude/ -I${ISL_INCLUDE_DIRECTORY} -o build/final
	@echo; echo;
	@DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:${HALIDE_LIB_DIRECTORY} build/final


clean:
	rm -rf LLVM_generated_code.o src/*~ include/*~ build/*
