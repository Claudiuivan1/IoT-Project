APPLICATION = riot_lora

BOARD ?= b-l072z-lrwan1

# RIOT base directory path:
RIOTBASE ?= $(CURDIR)/../RIOT

BOARD_WITHOUT_LORAMAC_RX := \
    arduino-mega2560 \
    i-nucleo-lrwan1 \
    stm32f0discovery \
    waspmote-pro \

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

# LoRaMac support
ifeq (,$(filter $(BOARD),$(BOARD_WITHOUT_LORAMAC_RX)))
  USEMODULE += semtech_loramac_rx
endif

USEMODULE += auto_init_loramac
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

#USEMODULE += xtimer
# For uuid
USEMODULE += uuid
# For float
USEMODULE += printf_float
# To avoid to wait to send
CFLAGS=-DDISABLE_LORAMAC_DUTYCYCLE
# end additions

FEATURES_OPTIONAL += periph_eeprom

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
