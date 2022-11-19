BUILD_DIR := ./build
LOG_DIR := ./logs

all: build

setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LOG_DIR)

build: setup
	cd $(BUILD_DIR); cmake -DCMAKE_BUILD_TYPE=Release ..
	cd $(BUILD_DIR); make -j

debug: setup
	cd $(BUILD_DIR); cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd $(BUILD_DIR); make -j

.PHONY: clean setup

clean:
	rm -r $(BUILD_DIR)
	rm -r $(LOG_DIR)