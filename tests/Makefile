# Host-side regression tests for the NonVolatile library.
# No Arduino toolchain is needed: tests/avr/eeprom.h mocks the EEPROM.

CXX ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CPPFLAGS := -I. -I..

BUILD := build

BINARIES := \
	$(BUILD)/test_layout \
	$(BUILD)/test_counter \
	$(BUILD)/test_double_include \
	$(BUILD)/test_multitu \
	$(BUILD)/test_static \
	$(BUILD)/test_static_multitu \
	$(BUILD)/bounds_space \
	$(BUILD)/bounds_tail \
	$(BUILD)/bounds_address \
	$(BUILD)/bounds_fill

.PHONY: all test clean static-bounds-fail

all: $(BINARIES)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_layout: test_layout.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(BUILD)/test_counter: test_counter.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(BUILD)/test_double_include: test_double_include.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(BUILD)/test_multitu: test_multitu.cpp tu_a.cpp tu_b.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ test_multitu.cpp tu_a.cpp tu_b.cpp

$(BUILD)/test_static: test_static.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(BUILD)/test_static_multitu: test_static_multitu.cpp tu_static_a.cpp tu_static_b.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ test_static_multitu.cpp tu_static_a.cpp tu_static_b.cpp

$(BUILD)/bounds_space: test_bounds.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DNV_BOUNDS_SPACE -o $@ $<

$(BUILD)/bounds_tail: test_bounds.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DNV_BOUNDS_TAIL -o $@ $<

$(BUILD)/bounds_address: test_bounds.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DNV_BOUNDS_ADDRESS -o $@ $<

$(BUILD)/bounds_fill: test_bounds.cpp avr/eeprom.h ../NonVolatile.h | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DNV_BOUNDS_FILL -o $@ $<

test: all
	./$(BUILD)/test_layout
	./$(BUILD)/test_counter
	./$(BUILD)/test_double_include
	./$(BUILD)/test_multitu
	./$(BUILD)/test_static
	./$(BUILD)/test_static_multitu
	@for t in bounds_space bounds_tail bounds_address bounds_fill; do \
		status=0; \
		timeout 1 ./$(BUILD)/$$t || status=$$?; \
		if [ $$status -ne 124 ]; then \
			echo "FAIL $$t: expected the fatal hang (124), got $$status"; \
			exit 1; \
		fi; \
		echo "$$t: OK (fatal hang, no out-of-range access)"; \
	done
	@$(MAKE) --no-print-directory static-bounds-fail
	@echo "All tests passed"

static-bounds-fail: | $(BUILD)
	@if $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c test_static_bounds_fail.cpp -o /dev/null 2>$(BUILD)/static_bounds_fail.err; then \
		echo "FAIL: out-of-range static declaration compiled"; \
		exit 1; \
	else \
		if grep -q "does not fit in the EEPROM" $(BUILD)/static_bounds_fail.err; then \
			echo "static bounds: out-of-range declaration rejected (OK)"; \
		else \
			echo "FAIL: compile failed for the wrong reason"; \
			cat $(BUILD)/static_bounds_fail.err; \
			exit 1; \
		fi; \
	fi

clean:
	rm -rf $(BUILD)
