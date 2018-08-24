.PHONY: all integration regression unit clean

all: integration regression unit

integration:
	$(MAKE) -C integration

regression:
	$(MAKE) -C regression

unit:
	$(MAKE) -C unit

clean:
	$(MAKE) -C integration clean
	$(MAKE) -C regression clean
	$(MAKE) -C unit clean
