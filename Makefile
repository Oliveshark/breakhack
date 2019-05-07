all:
	@make -C _build/debug
.PHONY: all

release:
	@make -C _build/release
.PHONY: release

clean:
	@make clean -C _build/debug
	@make clean -C _build/release
.PHONY: clean

test:
	@make test -C _build/debug
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/release/breakhack
.PHONY: run

lint:
	@make lint -C _build/debug
.PHONY: lint

package:
	@make package -C _build/release
.PHONY: package
