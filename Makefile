BUILD_DIR := ./build

build:
	mkdir -p $(BUILD_DIR)
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build; make -j

debug:
	mkdir -p $(BUILD_DIR)
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build; make -j

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)