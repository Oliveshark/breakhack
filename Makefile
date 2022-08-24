all:
	@make -sC _build/debug
.PHONY: all

release:
	@make -sC _build/release
.PHONY: release

clean:
	@make -sC _build/debug clean
	@make -sC _build/release clean
.PHONY: clean

test:
	@make -sC _build/debug test
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/release/breakhack
.PHONY: playtest

lint:
	@make -sC _build/debug lint
.PHONY: lint

package:
	@make -sC _build/release package
.PHONY: package

setup:
	@mkdir -p _build/release
	@mkdir -p _build/debug
	@cd _build/debug/ && \
		cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ../.. && \
		cd -
	@cd _build/debug/ && \
		cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
		cd -
	@ln -s _build/debug/compile_commands.json
	@echo "Setup complete"
.PHONY: setup

teardown:
	@rm -rf _build
	@rm compile_commands.json
.PHONY: teardown
