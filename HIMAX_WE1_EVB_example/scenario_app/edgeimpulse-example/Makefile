ARC_TOOLCHAIN ?= gnu

CC := arc-elf32-gcc
AR := arc-elf32-ar
CXX := arc-elf32-g++
LD := arc-elf32-ld

SDK_PATH = ../../himax_we1_sdk
MLI_PATH = ../../third_party/arc_mli_package/bin/himax_arcem9d_r16/release
DEPEND_PATH = ../../third_party/mw_gnu_dependencies/gnu_depend_lib
TF_PATH = ./edge-impulse-sdk
MODEL_PATH = ./tflite-model

GEN_TOOL_DIR = ../../image_gen_linux
GEN_TOOL_NAME = image_gen_gnu

TFSRCS := \
$(wildcard edge-impulse-sdk/tensorflow/lite/micro/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/core/api/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/c/*.c) \
$(wildcard edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/kernels/*.cc) \
$(wildcard edge-impulse-sdk/tensorflow/lite/micro/kernels/arc_mli/*.cc) \
$(wildcard edge-impulse-sdk/porting/himax/*.cpp) \
$(wildcard tflite-model/*.cpp) \

SRC := \
main.cc \

TFOBJS := \
$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(TFSRCS))))

SRCOBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRC)))

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
-DCPU_ARC \
-DEI_CLASSIFIER_ALLOCATION_STATIC_HIMAX \
-DSCRATCH_MEM_Z_SIZE=0 \
-I. \
-I $(SDK_PATH) \
-I $(TF_PATH) \
-I $(MODEL_PATH) \
-I./edge-impulse-sdk/porting \
-I./edge-impulse-sdk/third_party/flatbuffers/include/ \
-I./edge-impulse-sdk/third_party/gemmlowp/ \
-I./edge-impulse-sdk/third_party/ruy/ \
-I../../third_party/arc_mli_package/include/ \


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
-DCPU_ARC \
-DEI_CLASSIFIER_ALLOCATION_STATIC_HIMAX \
-DSCRATCH_MEM_Z_SIZE=0 \
-include $(SDK_PATH)/core_config.h \
-I. \
-I $(SDK_PATH) \
-I $(TF_PATH) \
-I $(MODEL_PATH) \
-I./edge-impulse-sdk/porting \
-I./edge-impulse-sdk/third_party/flatbuffers/include/ \
-I./edge-impulse-sdk/third_party/gemmlowp/ \
-I./edge-impulse-sdk/third_party/ruy/ \
-I../../third_party/arc_mli_package/include/ \


LDFLAGS +=  -Wl,-lmli -Wl,-lmwdepend -Wl,-marcv2elfx -Wl,-Map=memory.map -Wl,--strip-debug -Wl,--stats,--gc-sections -Wl,--cref \
-L$(MLI_PATH) \
-L$(DEPEND_PATH) \
-L../../ \
-Wl,--start-group \
$(SDK_PATH)/libboard_open_socket.a \
$(SDK_PATH)/libcpuarc.a \
$(SDK_PATH)/libbss.a \
$(SDK_PATH)/libboard_socket.a \
$(SDK_PATH)/liblibcommon.a \
$(SDK_PATH)/liblibaudio.a \
$(SDK_PATH)/liblibsecurity.a \
$(SDK_PATH)/liblibsensordp.a \
$(SDK_PATH)/liblibtflm.a \
-Wl,--end-group \

#=============================================================
# Common rules
#=============================================================
.PHONY: all

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

#=================================================================
# Global rules
#=================================================================
all: edgeimpulse_example

patch:
	@sed -i 's/#pragma Bss(".Xdata")/#define __Xdata_attr __attribute__((section(".Xdata")))/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i -e 's/static int8_t scratch_mem_x\[SCRATCH_MEM_X_SIZE\];/#define _X  __Xdata_attr/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i -e '0,/#pragma Bss()/ s/#pragma Bss()/static int8_t _X scratch_mem_x[SCRATCH_MEM_X_SIZE];/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i 's/#pragma Bss(".Ydata")/#define __Ydata_attr __attribute__((section(".Ydata")))/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i 's/static int8_t scratch_mem_y\[SCRATCH_MEM_Y_SIZE\];/#define _Y  __Ydata_attr/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i -e '0,/#pragma Bss()/ s/#pragma Bss()/static int8_t _Y scratch_mem_y[SCRATCH_MEM_Y_SIZE];/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i 's/#pragma Bss(".Zdata")/#define __Zdata_attr __attribute__((section(".Zdata")))/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i 's/static int8_t scratch_mem_z\[SCRATCH_MEM_Z_SIZE\];/#define _Z  __Zdata_attr/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i -e '0,/#pragma Bss()/ s/#pragma Bss()/static int8_t _Z scratch_mem_z[SCRATCH_MEM_Z_SIZE];/' edge-impulse-sdk/tensorflow/lite/micro/kernels/scratch_buffers.cc
	@sed -i -e '/#pragma Bss(".tensor_arena")/{n;N;d}' tflite-model/trained_model_compiled.cpp
	@sed -i 's/#pragma Bss(".tensor_arena")/static uint8_t tensor_arena[kTensorArenaSize] __attribute__((section(".tensor_arena")));/' tflite-model/trained_model_compiled.cpp

edgeimpulse_example: MAP_NAME = edgeimpulse_example
edgeimpulse_example:
	${MAKE} -C . patch
	${MAKE} -C . edgeimpulse_example.elf

edgeimpulse_example.elf : $(TFOBJS) $(SRCOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TFOBJS) $(SRCOBJS) $(LDFLAGS)


clean:
	@echo 'cleaning'
	-@$(RM) $(TFOBJS) $(SRCOBJS)
	-@$(RM) *.elf
	-@$(RM) *.map


flash:
	@export PATH=$(shell pwd)/$(GEN_TOOL_DIR)/:$$PATH && \
	cp edgeimpulse_example.elf $(GEN_TOOL_DIR) && \
	cd $(GEN_TOOL_DIR) && \
	$(GEN_TOOL_NAME) -e edgeimpulse_example.elf -s 1024 -o edgeimpulse_example.img && \
	cp edgeimpulse_example*.img $(shell pwd) && \
	rm edgeimpulse_example.elf edgeimpulse_example*.img
