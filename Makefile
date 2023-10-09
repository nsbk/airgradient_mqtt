all:
	$(MAKE) -C tests

runtests:
	$(MAKE) -C tests runtests

clean:
	$(MAKE) -C tests clean
