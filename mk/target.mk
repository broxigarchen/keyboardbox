#MIT License
#
#Copyright (c) 2020 Brox Chen broxigarchen@gmail.com 
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

.PHONY: all clean install tags cleanall
#PATH
ifneq ($(EXCLUDE_DIR),)
EXCLUDE_PATH := $(foreach dir, $(EXCLUDE_DIR), $(patsubst %,-path '%' -prune -o, $(dir)))
endif
CSOURCES      := $(shell find $(PROJECT_ROOT) $(EXCLUDE_PATH) -name '*.c' -print)
CPPSOURCES    := $(shell find $(PROJECT_ROOT) $(EXCLUDE_PATH) -name '*.cpp' -print)
SOURCES       := $(CSOURCES) $(CPPSOURCES)
HEADERS       := $(shell find $(PROJECT_ROOT) $(EXCLUDE_PATH) -name '*.h')
INC_PATH      := $(dir $(abspath) $(HEADERS))
INC_PATH      += $(EXTRA_INC)
CSRC_PATH     := $(patsubst %/, %, $(dir $(abspath) $(CSOURCES)))
CPPSRC_PATH   := $(patsubst %/, %, $(dir $(abspath) $(CPPSOURCES)))
VPATH         := $(CSRC_PATH):$(CPPSRC_PATH)
_LIBS         := $(patsubst %, -l%, $(LIBS))

#Config
CXXFLAGS += -x c++ $(addprefix -I, $(INC_PATH))
CCFLAGS  += $(addprefix -I, $(INC_PATH))
ifneq ($(CPPSOURCES),) 
_LIBS += -lstdc++
endif

#Target
OBJS          := $(addsuffix .o, $(basename $(notdir $(SOURCES))))

ifeq ($(BUILD_DIR),)
BUILD_DIR      := $(PROJECT_ROOT)
_COBJS         := $(addsuffix .o, $(basename $(CSOURCES)))
_CPPOBJS       := $(addsuffix .o, $(basename $(CPPSOURCES)))
else
_COBJS         := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(CSOURCES)))))
_CPPOBJS       := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(CPPSOURCES)))))
endif

all: $(_COBJS) $(_CPPOBJS)
	$(CC) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET) $^ $(_LIBS)

$(_COBJS) : $(BUILD_DIR)/%.o : %.c $(HEADERS)
	$(CC) $(CCFLAGS) -c -o $@ $<

$(_CPPOBJS) : $(BUILD_DIR)/%.o : %.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c -o $@ $<

install:
	$(CP) -rvf $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR)

tags:
	$(RM) -f $(BUILD_DIR)/tags
	ctags -R -f $(BUILD_DIR)/tags

clean:
	@echo $(_CPPOBJS)
	@echo $(CPPSRC_PATH)
	$(RM) -rf $(BUILD_DIR)/*.o  $(BUILD_DIR)/$(TARGET)

cleanall:
	make clean
	$(RM) -f $(BUILD_DIR)/tags 
	 
