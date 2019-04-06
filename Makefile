all:
	@ninja -C _build/debug
.PHONY: all

release:
	@ninja -C _build/release
.PHONY: release

clean:
	@ninja clean -C _build/debug
	@ninja clean -C _build/release
.PHONY: clean

test:
	@ninja test -C _build/debug
.PHONY: test

run: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack
.PHONY: run

playtest: $(all)
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/release/breakhack
.PHONY: run

lint:
	@ninja lint -C _build/debug
.PHONY: lint

package:
	@ninja package -C _build/release
.PHONY: package
