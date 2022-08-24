all:
	@make -sC _build/debug
.PHONY: all

release:
	@make -sC _build/release
.PHONY: release

clean:
	@make clean -sC _build/debug
	@make clean -sC _build/release
.PHONY: clean

test:
	@make test -sC _build/debug
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/release/breakhack
.PHONY: playtest

lint:
	@make lint -sC _build/debug
.PHONY: lint

package:
	@make package -sC _build/release
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
.PHONY: teardown
