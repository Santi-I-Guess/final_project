# Two cars in every pot and a chicken in every garage.

CXX = g++
CXXFLAGS_DEBUG = -g
CXXFLAGS_WARN  = -Wall -Werror -Wextra -Wconversion -Wdouble-promotion \
				 -Wunreachable-code -Wshadow -Wpedantic -Wno-conversion
CPPVERSION = -std=c++17

ARCHIVE_EXTENSION = zip

ifeq ($(OS), Windows_NT)
	TARGET = $(PROJECT).exe
	DEL = del
	DEL_FLAGS = -Force
	ZIPPER = tar -a -c -f
	ZIP_NAME = $(PROJECT)_$(USERNAME).$(ARCHIVE_EXTENSION)
	Q =
	SEPERATOR=\\
else
	TARGET = $(PROJECT)
	DEL = rm
	DEL_FLAGS = -f
	ZIPPER = tar -acf
	Q= "
	ifeq ($(shell tar --version | grep -o "GNU tar"), GNU tar)
		ARCHIVE_EXTENSION = tar.gz
	endif
	ZIP_NAME = $(PROJECT)_$(USERNAME).$(ARCHIVE_EXTENSION)
	SEPERATOR=/
endif

# note to self: both () and {} can be used for variables
PROJECT = final_project
SRC_DIRS  = src$(SEPERATOR)assembler src$(SEPERATOR)misc \
			src$(SEPERATOR)simulator src
SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard ${dir}$(SEPERATOR)*.cpp))
H_FILES   = $(foreach dir, $(SRC_DIRS), $(wildcard ${dir}$(SEPERATOR)*.h))
OBJECTS = $(patsubst %.cpp, build$(SEPERATOR)%.o, $(notdir $(SRC_FILES)))
REZ_FILES = resources/pseudo_assembly_assembler.png
USERNAME  = santiago_sagastegui

# https://www.gnu.org/software/make/manual/html_node/File-Name-Functions.html
# this is why you read documentation, kids

BUILD_DIR = build

# order only prerequisite: only build if it does not exist
all: first second
first: | $(BUILD_DIR)
second: $(TARGET)

# note to self: makefiles understand forward slash for file seperators,
#     but windows doesn't, so anything that runns commands needs os specific
#     file path seperators
# note to self: for some reason, on windows, -p and -Force are interpreted as names instead
# of flags
$(BUILD_DIR):
	mkdir build

# VPATH variable specifies list of directories to search for prerequisites,
# if prerequisite is not in current directory
VPATH = $(SRC_DIRS)
build$(SEPERATOR)%.o: %.cpp
	@echo "building $(notdir $<)"
	@$(CXX) -o $@ -c $< $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN)

$(TARGET): $(OBJECTS)
	@echo "building $@"
	@$(CXX) -o $@ $^

# Remove-Item (del) has some weird positional things going on
clean: | $(BUILD_DIR)
	@echo cleaning target and objects
	@$(DEL) $(TARGET) $(wildcard build$(SEPERATOR)*.o) $(DEL_FLAGS)

depend:
	@sed --in-place=.bak '/^# DEPENDENCIES/,$$d' Makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> Makefile
	@$(CXX) -MM $(SRC_FILES) | sed "s/\([a-z_]*.o:\)/build\/\1/g" >> Makefile

submission: | $(BUILD_DIR)
	@echo "Creating submission file $(ZIP_NAME) ..."
	@echo "...Zipping source files:   $(SRC_FILES) ..."
	@echo "...Zipping header files:   $(H_FILES) ..."
	@echo "...Zipping resource files: $(REZ_FILES)..."
	@echo "...Zipping Makefile ..."
	$(ZIPPER) $(ZIP_NAME) $(SRC_FILES) $(H_FILES) $(REZ_FILES) Makefile
	@echo "...$(ZIP_NAME) done!"

debug:
	g++ $(SRC_FILES) -DDEBUG -fsanitize=address -o $(TARGET)

.PHONY: all clean depend submission debug
.DEFAULT: all

# DEPENDENCIES
build/blueprint.o: src/assembler/blueprint.cpp src/assembler/blueprint.h \
 src/assembler/../common_values.h
build/tokenizer.o: src/assembler/tokenizer.cpp src/assembler/tokenizer.h
build/translation.o: src/assembler/translation.cpp \
 src/assembler/../common_values.h src/assembler/translation.h
build/cmd_line_opts.o: src/misc/cmd_line_opts.cpp src/misc/cmd_line_opts.h
build/file_handling.o: src/misc/file_handling.cpp src/misc/file_handling.h
build/cpu_handle.o: src/simulator/cpu_handle.cpp \
 src/simulator/../common_values.h src/simulator/cpu_handle.h
build/auxiliary.o: src/auxiliary.cpp src/auxiliary.h src/common_values.h
build/main.o: src/main.cpp src/auxiliary.h src/common_values.h \
 src/assembler/blueprint.h src/assembler/../common_values.h \
 src/assembler/tokenizer.h src/assembler/translation.h \
 src/misc/cmd_line_opts.h src/misc/file_handling.h \
 src/simulator/cpu_handle.h
