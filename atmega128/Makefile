BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: configure help all clean

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) \
		$(if $(PROG_TYPE),-DPROG_TYPE=$(PROG_TYPE),) \
		$(if $(PROG_PORT),-DPROG_PORT=$(PROG_PORT),) \
		$(if $(PROG_BITCLOCK),-DPROG_BITCLOCK=$(PROG_BITCLOCK),)

help: configure
	$(MAKE) -C $(BUILD_DIR) help

all: configure
	$(CMAKE) --build $(BUILD_DIR)

clean:
	$(CMAKE) --build $(BUILD_DIR) --target clean

upload_%: configure
	$(CMAKE) --build $(BUILD_DIR) --target $@
