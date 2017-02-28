# This Makefile should be flexible enough to handle most simple projects.
# To adapt it, simply fill in the appropriate variables in the first 
# section. It may also be necessary to change the compiler.


# My editor is set to substitute spaces for tabs. so I prefer changing 
# the recipe prefix rather than adjusting my editor.
.RECIPEPREFIX =   

# With the exception of the program_OBJECTS variable, these must all be
# set separately for a given project. 
program_NAME := test

source_DIRECTORY := ./test_src
program_SOURCES := $(shell find $(source_DIRECTORY) -type f -name '*.cpp')
program_OBJECTS := ${program_SOURCES:.cpp=.o}
program_INCLUDES := .
program_LIBRARY_DIRS := 
program_LIBRARIES := 

# It might be preferable to read the specific compiler from the environment 
# or from the command line.
CXX = g++
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
# empty recipe to the dependence of %.o on %.cpp. I am not sure whether
# this step is really necessary.
%.o : %.cpp

# This line uses the created dependency file to control compilation.
# The creation of the dependency file takes place in the same step as
# compilation of the object file. We rename dependency files in a separate
# step in order to avoid leaving corrupted dependency files in case of a
# failed compilation. The SECONCDEXPANSION allows us to work with
# the automatic variables such as $@ in the dependency list.
.SECONDEXPANSION:
$(program_OBJECTS) : $$(patsubst %.o,%.cpp,$$@) $$(call DEPFILE,$$@,d)
	$(COMPILE.cc) $(OUTPUT_OPTION) $(patsubst %.o,%.cpp,$@)
	$(POSTCOMPILE)

# A no-op. If a target does not exist as a file, then it is counted as 
# having been updated when this rule is encountered.
# Thus if a dependency file does not exist, make will not quit with an 
# error. Instead it will execute the corresponding %.o rule. 
$(DEPDIR)/%.d: ;

# This prevents make from deleting dependency files, as it would normally
# delete intermediate files.
.PRECIOUS: $(DEPDIR)/%.d

clean : 
	- $(RM) $(program_NAME) 
	- $(RM) $(program_OBJECTS)
	- $(RM) -r $(DEPDIR)

# This directs make to read the following files as included makefiles.
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(program_SOURCES)))
