################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries/Wire/src/utility/subdir.mk
-include libraries/Wire/src/subdir.mk
-include libraries/WiFi/src/utility/subdir.mk
-include libraries/WiFi/src/subdir.mk
-include libraries/Ticker/subdir.mk
-include libraries/SPI/src/subdir.mk
-include libraries/MsTimer2/subdir.mk
-include libraries/FastLED/src/third_party/object_fled/src/subdir.mk
-include libraries/FastLED/src/third_party/espressif/led_strip/src/subdir.mk
-include libraries/FastLED/src/third_party/cq_kernel/subdir.mk
-include libraries/FastLED/src/sensors/subdir.mk
-include libraries/FastLED/src/platforms/wasm/ui/subdir.mk
-include libraries/FastLED/src/platforms/wasm/compiler/subdir.mk
-include libraries/FastLED/src/platforms/wasm/subdir.mk
-include libraries/FastLED/src/platforms/stub/subdir.mk
-include libraries/FastLED/src/platforms/esp/32/spi_ws2812/subdir.mk
-include libraries/FastLED/src/platforms/esp/32/rmt_5/subdir.mk
-include libraries/FastLED/src/platforms/esp/32/rmt_4/subdir.mk
-include libraries/FastLED/src/platforms/esp/32/i2s/subdir.mk
-include libraries/FastLED/src/platforms/esp/32/subdir.mk
-include libraries/FastLED/src/platforms/avr/subdir.mk
-include libraries/FastLED/src/platforms/arm/k20/subdir.mk
-include libraries/FastLED/src/fx/video/subdir.mk
-include libraries/FastLED/src/fx/2d/subdir.mk
-include libraries/FastLED/src/fx/subdir.mk
-include libraries/FastLED/src/fl/subdir.mk
-include libraries/FastLED/src/subdir.mk
-include libraries/ArduinoOTA/src/utility/subdir.mk
-include libraries/ArduinoOTA/src/subdir.mk
-include libraries/Adafruit_NeoPixel/subdir.mk
-include core/core/subdir.mk
-include SparkFun_SCD4x_Arduino_Library/src/subdir.mk
-include Adafruit_SSD1306/subdir.mk
-include Adafruit_BusIO/subdir.mk
-include Adafruit-GFX-Library/fontconvert/subdir.mk
-include Adafruit-GFX-Library/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
/home/julien/sloeber-workspace/CapteurCO2/Release/arduino.ar \

ELF += \
CapteurCO2.elf \

HEX += \
CapteurCO2.hex \



#bootloaderTest
BurnBootLoader: 
	@echo trying to burn bootloader ${bootloader.tool}
	${tools.${bootloader.tool}.erase.pattern}
	${tools.${bootloader.tool}.bootloader.pattern}

uploadWithBuild: all
	@echo trying to build and upload with upload tool ${upload.tool}
	${tools.${upload.tool}.upload.pattern}

uploadWithoutBuild: 
	@echo trying to upload without build with upload tool ${upload.tool}
	${tools.${upload.tool}.upload.pattern}
    
uploadWithProgrammerWithBuild: all
	@echo trying to build and upload with programmer ${program.tool}
	${tools.${program.tool}.program.pattern}

uploadWithProgrammerWithoutBuild: 
	@echo trying to upload with programmer ${program.tool} without build
	${tools.${program.tool}.program.pattern}

# All Target
all: CapteurCO2

# Tool invocations
CapteurCO2: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/home/julien/Sloeber/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-size" -A "/home/julien/sloeber-workspace/CapteurCO2/Release/CapteurCO2.elf"
	@echo 'Finished building target: $@'
	@echo ' '

/home/julien/sloeber-workspace/CapteurCO2/Release/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/home/julien/Sloeber/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc-ar" rcs  "/home/julien/sloeber-workspace/CapteurCO2/Release/arduino.ar" $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

CapteurCO2.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/home/julien/Sloeber/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=atmega328p   -o "/home/julien/sloeber-workspace/CapteurCO2/Release/CapteurCO2.elf"  $(LINK_OBJ)  $(AR) "-L/home/julien/sloeber-workspace/CapteurCO2/Release" -lm
	@echo 'Finished building: $@'
	@echo ' '

CapteurCO2.hex: $(ELF)
	@echo 'Do all objcopy commands'
	${recipe.objcopy.eep.pattern}
	${recipe.objcopy.hex.pattern}
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CCM_DEPS)$(C_UPPER_DEPS)$(PDE_DEPS)$(CXXM_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(C++M_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
