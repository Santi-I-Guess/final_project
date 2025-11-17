# Two cars in every pot and a chicken in every garage.

PROJECT = final_project
SRC_DIRS  = src/assembler src/misc src/simulator src
SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
H_FILES   = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.h))
REZ_FILES = resources/pseudo_assembly_assembler.png
USERNAME  = santiago_sagastegui

# use of sed and echo in template makefile implies dos systems have access
# to sed and echo. I think foreach is a native make command, so I don't have
# to worry about cross system compatibility there

CXX = g++
CXXFLAGS_DEBUG = -g
CXXFLAGS_WARN  = -Wall -Werror -Wextra -Wconversion -Wdouble-promotion \
				 -Wunreachable-code -Wshadow -Wpedantic
CPPVERSION = -std=c++17

ARCHIVE_EXTENSION = zip

ifeq ($(shell echo "Windows"), "Windows")
	TARGET = $(PROJECT).exe
	DEL = del
	ZIPPER = tar -a -c -f
	ZIP_NAME = $(PROJECT)_$(USERNAME).$(ARCHIVE_EXTENSION)
	Q =
else
	TARGET = $(PROJECT)
	DEL = rm -f
	ZIPPER = tar -acf
	Q= "

	ifeq ($(shell tar --version | grep -o "GNU tar"), GNU tar)
		ARCHIVE_EXTENSION = tar.gz
	endif

	ZIP_NAME = $(PROJECT)_$(USERNAME).$(ARCHIVE_EXTENSION)
endif


# needs to stay like this for suffix rule to work properly
OBJECTS = $(SRC_FILES:src/%.cpp=build/%.o)

all: $(TARGET)

# We use objects in the rule dependencies, but not the g++ call,
# since OBJECTS holds the build paths before the text replacement
$(TARGET): $(OBJECTS)
	$(CXX) -o $@  $(wildcard build/*.o)

# eval: the cheap trick to runtime evaluation
build/%.o: src/%.cpp
	$(eval TMP = $(shell echo $^ | sed "s/.*\/\([a-z_]\+\).cpp/build\/\1.o/"))
	@echo "$(TMP) <- $<"
	@$(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) -o $(TMP) -c $<

clean:
	$(DEL) $(TARGET) $(OBJECTS)

depend:
	@sed --in-place=.bak '/^# DEPENDENCIES/,$$d' Makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> Makefile
	@$(CXX) -MM $(SRC_FILES) >> Makefile

# last time i used mkdir -p, i wasn't entirely sure it was DOS compatible
BUILD_DIRS = build build/assembler build/misc build/simulator
submission:
	@echo "Creating submission file $(ZIP_NAME) ..."
	@echo "...Zipping source files:   $(SRC_FILES) ..."
	@echo "...Zipping header files:   $(H_FILES) ..."
	@echo "...Zipping resource files: $(REZ_FILES)..."
	@echo "...Zipping Makefile ..."
	$(ZIPPER) $(ZIP_NAME) $(SRC_FILES) $(H_FILES) $(REZ_FILES) $(BUILD_DIRS) \
		Makefile
	@echo "...$(ZIP_NAME) done!"

debug:
	g++ $(SRC_FILES) -DDEBUG -fsanitize=address -o $(TARGET)

.PHONY: all clean depend submission debug
.DEFAULT: all first

# DEPENDENCIES
main.o: src/main.cpp src/assembler/blueprint.h \
 src/assembler/common_values.h src/assembler/common_values.h \
 src/assembler/tokenizer.h src/assembler/translation.h \
 src/misc/cmd_line_opts.h src/misc/file_handling.h
blueprint.o: src/assembler/blueprint.cpp src/assembler/blueprint.h \
 src/assembler/common_values.h
tokenizer.o: src/assembler/tokenizer.cpp src/assembler/tokenizer.h
translation.o: src/assembler/translation.cpp \
 src/assembler/common_values.h src/assembler/translation.h
cmd_line_opts.o: src/misc/cmd_line_opts.cpp src/misc/cmd_line_opts.h
file_handling.o: src/misc/file_handling.cpp src/misc/file_handling.h
