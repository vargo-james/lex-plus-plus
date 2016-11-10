.RECIPEPREFIX =   

program_NAME := test
program_SOURCES := test.cpp
program_OBJECTS := ${program_SOURCES:.cpp=.o}
program_INCLUDES :=
program_LIBRARY_DIRS := 
program_LIBRARIES := 

CXX = g++-4.9
CXXFLAGS += -std=c++14 -g -Wall
CPPFLAGS += $(foreach include_dir, $(program_INCLUDES), -I$(include_dir))
LDFLAGS += $(foreach library_dir, $(program_LIBRARY_DIRS), -L$(library_dir))
LDFLAGS += $(foreach library, $(program_LIBRARY_DIRS), -l$(library))

DEPDIR = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

.PHONY: all clean

all: $(program_NAME)

$(program_NAME): $(program_OBJECTS)
	$(LINK.cc) $^ -o $@

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

clean : 
	@- $(RM) $(program_NAME) 
	@- $(RM) $(program_OBJECTS)

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(program_SOURCES)))
