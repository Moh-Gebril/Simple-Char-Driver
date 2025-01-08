obj-m += chardev.o

# Kernel source directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Build output directory
BUILDDIR := build

# Documentation output directory
DOCSDIR := docs

all:
	mkdir -p $(BUILDDIR)
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	mv *.ko $(BUILDDIR)
	find $(BUILDDIR) -maxdepth 1 -type f -name '.*' -exec rm -f {} \;
	rm -f .chardev* .Module* .modules* *.o *.d *.mod *.mod.c Module.order Module.symvers modules.order

# Target for generating documentation
docs:
	mkdir -p $(DOCSDIR)
	doxygen -g $(DOCSDIR)/Doxyfile
	sed -i 's|INPUT = .*|INPUT = ../chardev.c|' $(DOCSDIR)/Doxyfile
	sed -i 's|OUTPUT_DIRECTORY = .*|OUTPUT_DIRECTORY = $(DOCSDIR)|' $(DOCSDIR)/Doxyfile
	sed -i 's|GENERATE_HTML = .*|GENERATE_HTML = YES|' $(DOCSDIR)/Doxyfile
	sed -i 's|EXTRACT_ALL = .*|EXTRACT_ALL = YES|' $(DOCSDIR)/Doxyfile
	cp chardev.c $(DOCSDIR)
	doxygen $(DOCSDIR)/Doxyfile
	mv latex/ $(DOCSDIR)
	mv html/ $(DOCSDIR)
	$(MAKE) -C $(DOCSDIR)/latex all
	rm -rf $(DOCSDIR)/*.c


clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(BUILDDIR)/* $(DOCSDIR)
