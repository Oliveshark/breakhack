all:
	@make -sC _build
.PHONY: all

clean:
	@make clean -sC _build
.PHONY: clean

test:
	@make test -sC _build
.PHONY: test

run: $(all)
	@./build/breakhack
.PHONY: run

lint:
	@make lint -sC _build
.PHONY: lint
