
# note to self: manually update SRC_FILES and H_FILES when done
# note to self: it almost looks like SRC_FILES and H_FILES aren't
# properly being defined...

PROJECT = final_project
SRC_FILES = src/misc/file_handling.cpp \
			src/misc/cmd_line_opts.cpp \
			src/main.cpp \
			src/assembler/tokenizer.cpp \
			src/assembler/translation.cpp \
			src/assembler/blueprint.cpp
H_FILES = src/misc/file_handling.h \
		  src/misc/cmd_line_opts.h \
		  src/assembler/blueprint.h \
		  src/assembler/translation.h \
		  src/assembler/common_values.h \
		  src/assembler/tokenizer.h
REZ_FILES =
USERNAME  = santiago_sagastegui

SRC_FILES = 
H_FILES =
BUILD_DIR = build

CXX = g++
CXXFLAGS_DEBUG = -g
CXXFLAGS_WARN  = -Wall -Werror -Wextra -Wconversion -Wdouble-promotion \
				 -Wunreachable-code -Wshadow -Wpedantic
CPPVERSION = -std=c++17
OBJECTS = build/blueprint.o \
		  build/cmd_line_opts.o \
		  build/file_handling.o \
		  build/main.o \
		  build/tokenizer.o \
		  build/translation.o
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

# note to self: apparently, make evaluates all variables before
# it goes through targets and rules, so i must have all objects (from
# differing source files) determined
all:
	mkdir -p build
	make -C src/assembler
	make -C src/misc
	$(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) \
		-o build/main.o -c src/main.cpp
	$(CXX) -o $(TARGET) $(OBJECTS)

# $(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) -o $@ -c $<

# how do i automatically compile OBJECTS_2 and OBJECTS_3 without
# redefining build/%.o?

clean:
	$(DEL) $(TARGET) $(OBJECTS)

# this will no longer work
depend:
	@sed --in-place=.bak '/^# DEPENDENCIES/,$$d' Makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> Makefile
	@$(CXX) -MM $(SRC_FILES) >> Makefile

submission:
	@echo "Creating submission file $(ZIP_NAME) ..."
	@echo "...Zipping source files:   $(SRC_FILES) ..."
	@echo "...Zipping header files:   $(H_FILES) ..."
	@echo "...Zipping resource files: $(REZ_FILES)..."
	@echo "...Zipping Makefile ..."
	$(ZIPPER) $(ZIP_NAME) $(SRC_FILES) $(H_FILES) $(REZ_FILES) $(BUILD_DIR) \
		Makefile
	@echo "...$(ZIP_NAME) done!"

debug:
	g++ $(SRC_FILES) -DDEBUG -o $(TARGET)

.PHONY: all clean depend submission debug
.DEFAULT: all

# DEPENDENCIES
