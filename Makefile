# This Makefile should be flexible enough to handle most simple projects.
# To adapt it, simply fill in the appropriate variables in the first 
# section. It may also be necessary to change the compiler.


# My editor is set to substitute spaces for tabs. so I prefer to change 
# the recipe prefix to adjustig my editor.
.RECIPEPREFIX =   

# With the exception of the program_OBJECTS variable. These must all be
# set separately for a given project. 
program_NAME := test
program_SOURCES := $(wildcard *.cpp) $(wildcard matcher_test/*.cpp) \
	$(wildcard ttest/*.cpp)
program_OBJECTS := ${program_SOURCES:.cpp=.o}
program_INCLUDES := .
program_LIBRARY_DIRS := 
program_LIBRARIES := 

# It might be preferable to read the specific compiler from the environment 
# or from the command line.
CXX = g++-4.9
CXXFLAGS += -std=c++14 -g -Wall
CPPFLAGS += $(foreach include_dir, $(program_INCLUDES), -I$(include_dir))
LDFLAGS += $(foreach library_dir, $(program_LIBRARY_DIRS), -L$(library_dir))
LDFLAGS += $(foreach library, $(program_LIBRARY_DIRS), -l$(library))

# In this Makefile the header file dependencies are generated automatically
# using the compiler. The flags instructing it to do so are contained in 
# the DEPFLAGS variable.
DEPDIR = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

.PHONY: all clean

all: $(program_NAME)

$(program_NAME): $(program_OBJECTS)
	$(LINK.cc) $^ -o $@

# The first line overrides the default behavior of make by attaching an
# empty recipe to the dependence of %.o on %.cpp.
%.o : %.cpp
# This line uses the created dependency file to control compilation.
# The creation of the dependency file takes place in the same step as
# compilation of the object file. We rename dependency files in a separate
# step in order to avoid leaving corrupted dependency files in case of a
# failed compilation.
%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

matcher_test/%.o : matcher_test/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

ttest/%.o : ttest/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
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
	@- $(RM) $(program_NAME) 
	@- $(RM) $(program_OBJECTS)

# This directs make to read the following files as included makefiles.
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(program_SOURCES)))
