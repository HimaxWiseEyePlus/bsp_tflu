#=============================================================
# Toolchain definitions
#=============================================================
ARC_TOOLCHAIN ?= mwdt
#ARC_TOOLCHAIN ?= gnu

ifeq ($(ARC_TOOLCHAIN), mwdt)
	CC = ccac
	AR = arac
	CXX = ccac
	LD = ccac
else ifeq ($(ARC_TOOLCHAIN), gnu)
	CC := arc-elf32-gcc
	AR := arc-elf32-ar
	CXX := arc-elf32-g++
	LD := arc-elf32-ld
endif


RM = rm -rf
DL = curl -Ls -f
UNZIP = unzip 

SDK_LINK = https://www.himax.com.tw/we-i/himax_we1_sdk_v16.zip
SDK_LOC = .
SDK_NAME = sdk.zip

TOOL_LINK = https://www.himax.com.tw/we-i/image_gen_linux_v2_1_10.zip
TOOL_LOC = .
TOOL_NAME = image_gen.zip

DEPEND_LINK = https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_mli/releases/download/Release_1.1/mw_gnu_dependencies.zip
DEPEND_LOC = third_party/
DEPEND_NAME = gnu_dependencies.zip

GEN_TOOL_DIR = image_gen_linux
ifeq ($(ARC_TOOLCHAIN), mwdt)
GEN_TOOL_NAME = image_gen
else ifeq ($(ARC_TOOLCHAIN), gnu)
GEN_TOOL_NAME = image_gen_gnu
endif
#=============================================================
# Files and directories
#=============================================================

SRCS := \

qwiic_ccs811_bme280_SRCS := \
scenario_app/qwiic_ccs811_bme280/main.cc \

qwiic_ms8607_SRCS := \
scenario_app/qwiic_ms8607/main.cc \

qwiic_ms8607_sparkfun_SRCS := \
scenario_app/qwiic_ms8607/sparkfun/SparkFun_PHT_MS8607_Arduino_Library.cc \
scenario_app/qwiic_ms8607/sparkfun/main.cc \

qwiic_ccs811_bme280_sparkfun_SRCS := \
scenario_app/qwiic_ccs811_bme280/sparkfun/SparkFunBME280.cc \
scenario_app/qwiic_ccs811_bme280/sparkfun/SparkFunCCS811.cc \
scenario_app/qwiic_ccs811_bme280/sparkfun/main.cc \

OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRCS)))

qwiic_ccs811_bme280_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ccs811_bme280_SRCS)))

qwiic_ms8607_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ms8607_SRCS)))

qwiic_ms8607_sparkfun_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ms8607_sparkfun_SRCS)))

qwiic_ccs811_bme280_sparkfun_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ccs811_bme280_sparkfun_SRCS)))

#=============================================================
# Applications settings
#=============================================================

DEPEND_PATH = third_party/mw_gnu_dependencies/gnu_depend_lib
SDK_PATH = himax_we1_sdk
LIB_PATH = lib

ifeq ($(ARC_TOOLCHAIN), mwdt)
CXXFLAGS+= \
-Wstrict-aliasing \
-DTF_LITE_STATIC_MEMORY \
-Wsign-compare \
-Wdouble-promotion \
-Wshadow \
-Wunused-function \
-DNDEBUG \
-O3 \
-fno-rtti \
-DSCRATCH_MEM_Z_SIZE=0x10000 \
-DNDEBUG \
-g \
-DCPU_ARC \
-Hnosdata \
-DTF_LITE_STATIC_MEMORY \
-tcf=$(SDK_PATH)/arcem9d_wei_r16.tcf \
-Hnocopyr \
-Hpurge \
-Hcl \
-fslp-vectorize-aggressive \
-ffunction-sections \
-fdata-sections \
-tcf_core_config \
-I. \
-I./$(SDK_PATH) \
-I./$(LIB_PATH) \


CCFLAGS+= \
-DTF_LITE_STATIC_MEMORY \
-DNDEBUG \
-O3 \
-DNDEBUG \
-g \
-DCPU_ARC \
-Hnosdata \
-DTF_LITE_STATIC_MEMORY \
-tcf=$(SDK_PATH)/arcem9d_wei_r16.tcf \
-Hnocopyr \
-Hpurge \
-Hcl \
-fslp-vectorize-aggressive \
-ffunction-sections \
-fdata-sections \
-tcf_core_config \
-I. \
-I. $(SDK_PATH)\
-I. $(LIB_PATH) \

LDFLAGS+= \
-Hheap=8192 \
-Hnocopyr \
-m \
-Hldopt=-Coutput=$(MAP_NAME).map \
$(SDK_PATH)/memory.lcf \
-Hldopt=-Bgrouplib \
$(SDK_PATH)/libcpuarc.a \
$(SDK_PATH)/libbss.a \
$(SDK_PATH)/libboard_socket.a \
$(SDK_PATH)/liblibcommon.a \
$(SDK_PATH)/liblibaudio.a \
$(SDK_PATH)/liblibsecurity.a \
$(SDK_PATH)/liblibsensordp.a \
$(SDK_PATH)/liblibtflm.a

else ifeq ($(ARC_TOOLCHAIN), gnu)
CXXFLAGS += \
-fno-rtti \
-fno-exceptions \
-fno-threadsafe-statics \
-fno-unwind-tables \
-ffunction-sections \
-fdata-sections \
-fmessage-length=0 \
-DTF_LITE_STATIC_MEMORY \
-DTF_LITE_DISABLE_X86_NEON \
-O3 \
-Wsign-compare \
-Wdouble-promotion \
-Wshadow \
-Wunused-variable \
-Wmissing-field-initializers \
-Wunused-function \
-Wswitch \
-Wvla \
-Wall \
-Wextra \
-Wstrict-aliasing \
-Wno-unused-parameter \
-DREDUCE_CODESIZE \
-mxy \
-include $(SDK_PATH)/core_config.h \
-mcpu=em4_fpus \
-mlittle-endian \
-mcode-density \
-mdiv-rem \
-mswap \
-mnorm \
-mmpy-option=6 \
-mbarrel-shifter \
-mfpu=fpus_all \
-I. \
-I./$(SDK_PATH) \
-I./$(LIB_PATH) \
-DSCRATCH_MEM_Z_SIZE=0x10000 \


CCFLAGS+= \
-mcpu=em4_fpus \
-mlittle-endian \
-mcode-density \
-mdiv-rem \
-mswap \
-mnorm \
-mmpy-option=6 \
-mbarrel-shifter \
-mfpu=fpus_all \
-fno-unwind-tables \
-ffunction-sections \
-fdata-sections \
-fmessage-length=0 \
-DTF_LITE_STATIC_MEMORY \
-DTF_LITE_DISABLE_X86_NEON \
-O3 \
-DREDUCE_CODESIZE \
-mxy \
-include $(SDK_PATH)/core_config.h \
-I. \
-I./$(SDK_PATH) \
-I./$(LIB_PATH) \

LDFLAGS +=  -Wl,-lmwdepend -Wl,-marcv2elfx -Wl,-Map=memory.map -Wl,--strip-debug -Wl,--stats,--gc-sections -Wl,--cref \
-L$(DEPEND_PATH) \
-L$(SDK_PATH) \
-Wl,--start-group \
$(SDK_PATH)/libcpuarc.a \
$(SDK_PATH)/libbss.a \
$(SDK_PATH)/libboard_socket.a \
$(SDK_PATH)/liblibcommon.a \
$(SDK_PATH)/liblibaudio.a \
$(SDK_PATH)/liblibsecurity.a \
$(SDK_PATH)/liblibsensordp.a \
$(SDK_PATH)/liblibtflm.a \
-Wl,--end-group \

endif # ARC_TOOLCHAIN


#=============================================================
# Common rules
#=============================================================
.PHONY: all qwiic_ccs811_bme280 qwiic_ms8607

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(EXT_CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $(EXT_CFLAGS) $(INCLUDES) -c $< -o $@


#=================================================================
# Global rules
#=================================================================
all: 
	$(error "please specific example=")

qwiic_ccs811_bme280: MAP_NAME = qwiic_ccs811_bme280
qwiic_ccs811_bme280: qwiic_ccs811_bme280.elf

qwiic_ccs811_bme280.elf : $(OBJS) $(qwiic_ccs811_bme280_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(qwiic_ccs811_bme280_OBJS) $(LDFLAGS)

qwiic_ms8607: MAP_NAME = qwiic_ms8607
qwiic_ms8607: qwiic_ms8607.elf

qwiic_ms8607.elf : $(OBJS) $(qwiic_ms8607_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(qwiic_ms8607_OBJS) $(LDFLAGS)

qwiic_ccs811_bme280_sparkfun: MAP_NAME = qwiic_ccs811_bme280_sparkfun
qwiic_ccs811_bme280_sparkfun: qwiic_ccs811_bme280_sparkfun.elf

qwiic_ccs811_bme280_sparkfun.elf : $(OBJS) $(qwiic_ccs811_bme280_sparkfun_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(qwiic_ccs811_bme280_sparkfun_OBJS) $(LDFLAGS)

qwiic_ms8607_sparkfun: MAP_NAME = qwiic_ms8607_sparkfun
qwiic_ms8607_sparkfun: qwiic_ms8607_sparkfun.elf

qwiic_ms8607_sparkfun.elf : $(OBJS) $(qwiic_ms8607_sparkfun_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(qwiic_ms8607_sparkfun_OBJS) $(LDFLAGS)
		
clean:
	@echo 'cleaning'
	-@$(RM) $(OBJS) $(qwiic_ccs811_bme280_OBJS) $(qwiic_ms8607_OBJS) ${qwiic_ms8607_sparkfun_OBJS} ${qwiic_ccs811_bme280_sparkfun_OBJS}
	-@$(RM) *.elf
	-@$(RM) *.map
	
download: $(MODEL_LOC)
	@echo 'downloading'
	@$(DL) $(SDK_LINK)  -o $(SDK_NAME)
	@$(DL) $(TOOL_LINK)  -o $(TOOL_NAME)
	@$(DL) $(DEPEND_LINK)  -o $(DEPEND_NAME)
	@$(UNZIP) -o $(SDK_NAME) -d $(SDK_LOC) 
	@$(UNZIP) -o $(TOOL_NAME) -d $(TOOL_LOC)	
	@$(UNZIP) -o $(DEPEND_NAME) -d $(DEPEND_LOC)
	@$(RM) $(SDK_NAME)
	@$(RM) $(TOOL_NAME)	
	@$(RM) $(DEPEND_NAME)
	
$(MODEL_LOC):
	@mkdir -p $@

ifeq ($(ARC_TOOLCHAIN), mwdt)
flash:
ifdef example
	@export PATH=$(shell pwd)/$(GEN_TOOL_DIR)/:$$PATH && \
	cp $(example).elf $(example).map $(GEN_TOOL_DIR) && \
	cd $(GEN_TOOL_DIR) && \
	$(GEN_TOOL_NAME) -e $(example).elf -m $(example).map -o $(example).img -s 1024 && \
	cp $(example)*.img .. && \
	rm $(example).elf $(example).map $(example)*.img
else
	$(error "please specific example=")
endif
else ifeq ($(ARC_TOOLCHAIN), gnu)
flash:
ifdef example
	@export PATH=$(shell pwd)/$(GEN_TOOL_DIR)/:$$PATH && \
	cp $(example).elf $(GEN_TOOL_DIR) && \
	cd $(GEN_TOOL_DIR) && \
	$(GEN_TOOL_NAME) -e $(example).elf -s 1024 -o $(example).img && \
	cp $(example)*.img .. && \
	rm $(example).elf $(example)*.img
else
	$(error "please specific example=")
endif

endif 	
