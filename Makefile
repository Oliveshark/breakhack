all:
	@make -sC _build/debug
	@make -sC _build/release
.PHONY: all

clean:
	@make clean -sC _build/debug
	@make clean -sC _build/release
.PHONY: clean

test:
	@make test -sC _build/debug
.PHONY: test

run: $(all)
	@./_build/debug/breakhack
.PHONY: run

lint:
	@make lint -sC _build/debug
.PHONY: lint

package:
	@make package -sC _build/release
.PHONY: package
