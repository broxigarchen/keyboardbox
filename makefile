
include   mk/common.mk

BUILD_DIR = $(PROJECT_ROOT)/build
EXCLUDE_DIR += $(PROJECT_ROOT)/utils $(PROJECT_ROOT)/actors/templateActor
EXCLUDE_INC += 
EXTRA_INC += 

CXXFLAGS += -Wall
CCFLAGS  += -Wall
LDFLAGS  += -pthread
TARGET = keyboardbox
LIBS =

include  mk/target.mk

