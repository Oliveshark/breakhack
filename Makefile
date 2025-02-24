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
	@if [ -d build/win-release ]; then cmake --build build/win-release --target clean; fi
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
	@cpack --config build/release/CPackConfig.cmake -G TGZ
	@cpack --config build/win-release/CPackConfig.cmake -G TGZ
.PHONY: package

install:
	@cmake --build build/release --target install
.PHONY: install

setup:
	@mkdir -p build/debug
	@cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -GNinja
	@mkdir -p build/release
	@cmake -B build/release -DCMAKE_BUILD_TYPE=Release -GNinja
	@ln -fs build/debug/compile_commands.json
	@echo "Setup complete"
.PHONY: setup

setup-win:
	@mkdir -p build/win-release
	@cmake -B build/win-release \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_TOOLCHAIN_FILE=build_deps/toolchains/mingw-w64-i686.cmake \
		-DSDLMIXER_VENDORED=ON \
		-DSDLTTF_VENDORED=ON \
		-GNinja
.PHONY: setup-win

teardown:
	@rm -rf build
	@rm -f compile_commands.json
.PHONY: teardown
