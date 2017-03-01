# vim: set tabstop=8:softtabstop=8:shiftwidth=8:noexpandtab

# This Makefile should be flexible enough to handle almost all simple projects.
# To adapt it, simply fill in the appropriate variables in the first 
# section. I don't know whether non-GNU compilers support the automatic
# dependency generation.

# PROJECT SPECIFIC VARIABLES
COMPILER = g++	#Other compilers might not work?
program_NAME := test
source_DIRECTORY := ./src
program_INCLUDES := .
program_LIBRARY_DIRS := 
program_LIBRARIES := 

# These variables are automatically set.
# Note that all source files in the source directory will be selected for
# compilation and linking. If this is not desirable, an alternate script
# must be given to produce all the source files.
program_SOURCES := $(shell find $(source_DIRECTORY) -type f -name '*.cpp')
program_OBJECTS := ${program_SOURCES:.cpp=.o}
CXX = $(COMPILER)
CXXFLAGS += -std=c++14 -g -Wall -pedantic
CPPFLAGS += $(foreach include_dir, $(program_INCLUDES), -I$(include_dir))
LDFLAGS += $(foreach library_dir, $(program_LIBRARY_DIRS), -L$(library_dir))
LDFLAGS += $(foreach library, $(program_LIBRARY_DIRS), -l$(library))

# In this Makefile the header file dependencies are generated automatically
# using the compiler. The flags instructing it to do so are contained in 
# the DEPFLAGS variable.
DEPDIR = .d
$(shell mkdir -p $(dir $(addprefix $(DEPDIR)/, $(program_SOURCES))) >/dev/null)

# This is a function that produces the pathname for the dependency file
# with suffix $(2) given object $(1).
DEPFILE = $(DEPDIR)/$(basename $(1)).$(2)

DEPFLAGS = -MT $@ -MMD -MP -MF $(call DEPFILE,$@,Td)
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(call DEPFILE,$@,Td) $(call DEPFILE,$@,d) 

.PHONY: all clean

all: $(program_NAME)

# This is the recipe to link the objects into an executable.
$(program_NAME): $(program_OBJECTS)
	$(LINK.cc) $^ -o $@

# This recipe overrides the default behavior of make by attaching an
# empty recipe to the dependence of %.o on %.cpp. I am honestly not sure 
# whether this step is really necessary.
#%.o : %.cpp

# This line establishes the dependency relationship between an object
# file and its corresponding source and dependency files.
#
# The usual %.o %.cpp pattern matching is not robust enough to handle
# source files in all possible subdirectories.
# Note the gnu make extension SECONDEXPANSION. Upon the first expansion, 
# this rule is expanded to a list of rules for each source file. But in the 
# dependencies, only one layer of $ calling has been done. Thus in the 
# second expansion, the dependencies are fully resolved. This makes it 
# possible to use the automatic varialbe $@ in the dependency list.
# By the time of the second expansion,the first expansion has resolved
# exactly what the target is. So in the second expansion, we get exactly
# the source file and dependency file for that target. 
#
# The two steps in the # recipe do:
# 	1. Compilation and writing a temporary dependency file.
# 	2. Moving the temporary dependency to its "permanent" location.
# The two stage process ensures that dependencies are not corrupted by 
# failed compilation.
#
# Note that this does not set up the dependency upon header files. That
# is accomplished with the -include at the end of the Makefile.
.SECONDEXPANSION:
$(program_OBJECTS) : $$(patsubst %.o,%.cpp,$$@) $$(call DEPFILE,$$@,d)
	$(COMPILE.cc) $(OUTPUT_OPTION) $(patsubst %.o,%.cpp,$@)
	$(POSTCOMPILE)

# A no-op. If a dependency file does not exist, then it is counted as 
# having been updated when this rule is encountered.
# Thus if a dependency file does not exist, make will not quit with an 
# error. Instead it will execute the rule above to create
# the corresponding object file. In the process it will create the
# dependency file for the next time make is invoked.
$(DEPDIR)/%.d: ;

# This prevents make from deleting dependency files, as it would normally
# delete intermediate files.
.PRECIOUS: $(DEPDIR)/%.d

clean : 
	- $(RM) $(program_NAME) 
	- $(RM) $(program_OBJECTS)
	- $(RM) -r $(DEPDIR)

# This directs make to read the following files as included makefiles.
# These rules actually establish the dependency of source file on headers.
# They are automatically created by the compiler (g++).
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(program_SOURCES)))
