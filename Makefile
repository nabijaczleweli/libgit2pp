# The MIT License (MIT)

# Copyright (c) 2016 nabijaczleweli

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


include configMakefile


SOURCES = $(sort $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp) $(wildcard src/**/**/**/*.cpp))
OBJECTS = $(patsubst src/%.cpp,$(BUILD)/obj/%$(OBJ),$(SOURCES))
TEST_SOURCES = $(sort $(wildcard test/*.cpp) $(wildcard test/**/*.cpp) $(wildcard test/**/**/*.cpp) $(wildcard test/**/**/**/*.cpp))
TEST_OBJECTS = $(patsubst test/%.cpp,$(BUILD)/test/obj/%$(OBJ),$(TEST_SOURCES))


.PHONY : clean all dll stlib tests run-tests

all : dll stlib tests

clean :
	rm -rf $(BUILD)

run-tests : tests
	@$(BUILD)/test/cpp-localiser-tests$(EXE) --use-colour yes

tests : $(BUILD)/test/cpp-localiser-tests$(EXE)
dll : $(BUILD)/$(PREDLL)cpp-localiser$(DLL)
stlib : $(BUILD)/libcpp-localiser$(ARCH)


$(BUILD)/$(PREDLL)cpp-localiser$(DLL) : $(OBJECTS)
	$(CXX) $(CXXAR) -shared $(PIC) -o$@ $^

$(BUILD)/libcpp-localiser$(ARCH) : $(OBJECTS)
	ar crs $@ $^

$(BUILD)/test/cpp-localiser-tests$(EXE) : $(TEST_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXAR) $(PIC) -o$@ $^


$(BUILD)/obj/%$(OBJ) : src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(PIC) -Iinclude -c -o$@ $^

$(BUILD)/test/obj/%$(OBJ) : test/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(PIC) -Iinclude -Iext/Catch/include -DCATCH_CONFIG_COLOUR_ANSI -c -o$@ $^
