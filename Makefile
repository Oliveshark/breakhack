all:
	@make -sC build/debug
.PHONY: all

release:
	@make -sC build/release
.PHONY: release

clean:
	@make -sC build/debug clean
	@make -sC build/release clean
.PHONY: clean

test:
	@make -sC build/debug test
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./build/release/breakhack
.PHONY: playtest

lint:
	@make -sC build/debug lint
.PHONY: lint

package:
	@make -sC build/release package
.PHONY: package

setup:
	@mkdir -p build/debug
	@mkdir -p build/release
	@cd build/debug/ && \
		cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ../.. && \
		cd -
	@cd build/release/ && \
		cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
		cd -
	@ln -s build/debug/compile_commands.json
	@echo "Setup complete"
.PHONY: setup

teardown:
	@rm -rf build
	@rm compile_commands.json
.PHONY: teardown
