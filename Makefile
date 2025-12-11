# Two cars in every pot and a chicken in every garage.

CXX = g++
CXXFLAGS_DEBUG = -g -Wmissing-include-dirs
CXXFLAGS_WARN  = -Wall
CPPVERSION = -std=c++17
USERNAME = santiago_sagastegui

ARCHIVE_EXTENSION = zip

# $(OS) is defined for windows machines, but not unix
# also, why is -Force a positional flag? I really dislike Powershell
# note to self: for some reason, on windows, -p and -Force are interpreted
# as names instead of flags if they're not at the end
ifeq ($(OS),Windows_NT)
	TARGET = $(PROJECT).exe
	DEL = del
	DEL_FLAGS = -Force
	ZIPPER = tar -a -c -f
	ZIP_NAME = $(PROJECT)_$(USERNAME).$(ARCHIVE_EXTENSION)
	Q =
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
endif

PROJECT = final_project
SRC_DIRS  = src/assembler \
			src/misc \
			src/simulator \
			src
SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard ${dir}/*.cpp))
H_FILES   = $(foreach dir, $(SRC_DIRS), $(wildcard ${dir}/*.h))
OBJECTS = $(patsubst %.cpp, build/%.o, $(notdir $(SRC_FILES)))
REZ_FILES = resources/pseudo_assembly_assembler.png \
			docs/Doxyfile \
			$(wildcard docs/*.md) \
			$(wildcard examples/*.pseudo) \
			$(wildcard testing/*.py) \
			$(wildcard testing/*.sh) \

# https://www.gnu.org/software/make/manual/html_node/File-Name-Functions.html

BUILD_DIR = build

# order only prerequisite: only build if it does not exist
all: first second
first: | $(BUILD_DIR)
second: $(TARGET)

$(BUILD_DIR):
	mkdir build

# VPATH variable specifies list of directories to search for prerequisites,
# if prerequisite is not in current directory
VPATH = $(SRC_DIRS)
build/%.o: %.cpp | $(BUILD_DIR)
	@echo "building $(notdir $<)"
	@$(CXX) -o $@ -c $< $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN)

$(TARGET): $(OBJECTS)
	@echo "building $@"
	@$(CXX) -o $@ $^ $(CXXFLAGS_DEBUG)

# Remove-Item (del) has some weird positional things going on
clean: | $(BUILD_DIR)
	@echo cleaning target
	@$(DEL) $(TARGET) $(DEL_FLAGS)
	@echo cleaning objects
	@$(DEL) $(wildcard build/*.o) $(DEL_FLAGS)


depend:
	@sed --in-place=.bak '/^# DEPENDENCIES$$/,$$d' Makefile
	@$(DEL) sed* $(DEL_FLAGS)
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

.PHONY: all clean depend submission
.DEFAULT: all

# DEPENDENCIES
build/assembler.o: src/assembler/assembler.cpp src/assembler/../common_values.h \
 src/assembler/assembler.h src/assembler/tokenizer.h
build/helper.o: src/assembler/helper.cpp src/assembler/../common_values.h \
 src/assembler/helper.h
build/tokenizer.o: src/assembler/tokenizer.cpp src/assembler/../common_values.h \
 src/assembler/helper.h src/assembler/tokenizer.h
build/cmd_line_opts.o: src/misc/cmd_line_opts.cpp src/misc/cmd_line_opts.h
build/file_handling.o: src/misc/file_handling.cpp src/misc/file_handling.h \
 src/misc/../common_values.h
build/cpu_handle.o: src/simulator/cpu_handle.cpp \
 src/simulator/../common_values.h src/simulator/cpu_handle.h \
 src/simulator/pal_debugger.h src/simulator/instructions.h
build/instructions.o: src/simulator/instructions.cpp \
 src/simulator/../common_values.h src/simulator/cpu_handle.h \
 src/simulator/instructions.h
build/pal_debugger.o: src/simulator/pal_debugger.cpp \
 src/simulator/pal_debugger.h src/simulator/cpu_handle.h \
 src/simulator/../common_values.h
build/common_values.o: src/common_values.cpp src/common_values.h
build/main.o: src/main.cpp src/assembler/assembler.h \
 src/assembler/../common_values.h src/assembler/tokenizer.h \
 src/common_values.h src/misc/cmd_line_opts.h src/misc/file_handling.h \
 src/misc/../common_values.h src/simulator/cpu_handle.h \
 src/simulator/../common_values.h
