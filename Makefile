all:
	$(MAKE) -C tests arduino_ide_dir=$(arduino_ide_dir)

runtests:
	$(MAKE) -C tests runtests arduino_ide_dir=$(arduino_ide_dir)

clean:
	$(MAKE) -C tests clean arduino_ide_dir=$(arduino_ide_dir)
