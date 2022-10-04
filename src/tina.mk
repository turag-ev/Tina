TINA_BASE := $(dir $(lastword $(MAKEFILE_LIST)))

TINA_INC := $(TINA_BASE)

TINA_CSRC := $(foreach MODULE,$(TINA_MODULES),$(wildcard $(TINA_BASE)/tina/$(MODULE)/*.c))
TINA_CPPSRC := $(foreach MODULE,$(TINA_MODULES),$(wildcard $(TINA_BASE)/tina++/$(MODULE)/*.cpp))

ifneq ($(TINA_PLATFORM),)
TINA_PLT_BASE := $(TINA_BASE)/platform/$(TINA_PLATFORM)

TINA_CSRC += $(shell find $(TINA_PLT_BASE) -type f -name "*.c")
TINA_CPPSRC += $(shell find $(TINA_PLT_BASE) -type f -name "*.cpp")
TINA_INC += $(TINA_PLT_BASE)/public
endif
