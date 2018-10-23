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

lint:
	@make lint -sC _build/debug
.PHONY: lint

package:
	@make package -sC _build/release
.PHONY: package
