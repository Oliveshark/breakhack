all:
	@cmake --build build/debug
.PHONY: all

release:
	@cmake --build build/release
.PHONY: release

windows:
	@cmake --build build/win-release
.PHONY: windows

clean:
	@cmake --build build/debug --target clean
	@cmake --build build/release --target clean
	@cmake --build build/win-release --target clean
.PHONY: clean

test:
	@cmake --build build/debug --target test
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./build/release/breakhack
.PHONY: playtest

lint:
	@cmake --build build/debug --target lint
.PHONY: lint

package:
	@cmake --build build/release --target package
.PHONY: package

setup:
	@mkdir -p build/debug
	@mkdir -p build/release
	@mkdir -p build/win-release
	@cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -GNinja
	@cmake -B build/release -DCMAKE_BUILD_TYPE=Release -GNinja
	@cmake -B build/win-release \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_TOOLCHAIN_FILE=build_deps/toolchains/mingw-w64-x86_64.cmake \
		-DSDL2MIXER_VENDORED=ON \
		-DSDL2TTF_VENDORED=ON \
		-GNinja
	@ln -fs build/debug/compile_commands.json
	@echo "Setup complete"
.PHONY: setup

teardown:
	@rm -rf build
	@rm compile_commands.json
.PHONY: teardown
