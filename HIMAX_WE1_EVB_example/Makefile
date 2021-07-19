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
CP = cp -r

SDK_LINK = https://www.himax.com.tw/we-i/himax_we1_sdk_v18.zip
SDK_LOC = .
SDK_NAME = sdk.zip

TOOL_LINK = https://www.himax.com.tw/we-i/image_gen_linux_v2_1_10.zip
TOOL_LOC = .
TOOL_NAME = image_gen.zip

DEPEND_LINK = https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_mli/releases/download/Release_1.1/mw_gnu_dependencies.zip
DEPEND_LOC =  third_party/
DEPEND_NAME = gnu_dependencies.zip

TF_LINK = https://github.com/HimaxWiseEyePlus/himax_tflm/archive/master.zip
TF_LOC = ./
TF_NAME = himax_tflm.zip

MODEL_LINK = https://www.himax.com.tw/we-i/himax_tflu_tree/model_setting_v03.zip
MODEL_LOC = tensorflow/lite/micro/tools/make/downloads/
MODEL_NAME = model.zip

LIB_LINK = https://www.himax.com.tw/we-i/himax_tflu_tree/third_party_lib_v03.zip
LIB_LOC = third_party/
LIB_NAME = lib.zip

GEN_TOOL_DIR = image_gen_linux
ifeq ($(ARC_TOOLCHAIN), mwdt)
GEN_TOOL_NAME = image_gen
else ifeq ($(ARC_TOOLCHAIN), gnu)
GEN_TOOL_NAME = image_gen_gnu
endif
#=============================================================
# Files and directories
#=============================================================

TFSRCS := \
tensorflow/lite/micro/simple_memory_allocator.cc \
tensorflow/lite/micro/mock_micro_graph.cc \
tensorflow/lite/micro/micro_allocator.cc \
tensorflow/lite/micro/all_ops_resolver.cc \
tensorflow/lite/micro/memory_helpers.cc \
tensorflow/lite/micro/micro_interpreter.cc \
tensorflow/lite/micro/micro_error_reporter.cc \
tensorflow/lite/micro/micro_time.cc \
tensorflow/lite/micro/recording_simple_memory_allocator.cc \
tensorflow/lite/micro/micro_string.cc \
tensorflow/lite/micro/micro_profiler.cc \
tensorflow/lite/micro/micro_utils.cc \
tensorflow/lite/micro/himax_we1_evb/debug_log.cc \
tensorflow/lite/micro/test_helpers.cc \
tensorflow/lite/micro/micro_graph.cc \
tensorflow/lite/micro/system_setup.cc \
tensorflow/lite/micro/recording_micro_allocator.cc \
tensorflow/lite/micro/memory_planner/linear_memory_planner.cc \
tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc \
tensorflow/lite/c/common.c \
tensorflow/lite/core/api/error_reporter.cc \
tensorflow/lite/core/api/flatbuffer_conversions.cc \
tensorflow/lite/core/api/op_resolver.cc \
tensorflow/lite/core/api/tensor_utils.cc \
tensorflow/lite/kernels/internal/quantization_util.cc \
tensorflow/lite/kernels/kernel_util.cc \
tensorflow/lite/schema/schema_utils.cc \
tensorflow/lite/micro/kernels/activations.cc \
tensorflow/lite/micro/kernels/add.cc \
tensorflow/lite/micro/kernels/add_n.cc \
tensorflow/lite/micro/kernels/arg_min_max.cc \
tensorflow/lite/micro/kernels/batch_to_space_nd.cc \
tensorflow/lite/micro/kernels/cast.cc \
tensorflow/lite/micro/kernels/ceil.cc \
tensorflow/lite/micro/kernels/circular_buffer.cc \
tensorflow/lite/micro/kernels/comparisons.cc \
tensorflow/lite/micro/kernels/concatenation.cc \
tensorflow/lite/micro/kernels/arc_mli/conv.cc \
tensorflow/lite/micro/kernels/conv_common.cc \
tensorflow/lite/micro/kernels/cumsum.cc \
tensorflow/lite/micro/kernels/depth_to_space.cc \
tensorflow/lite/micro/kernels/arc_mli/depthwise_conv.cc \
tensorflow/lite/micro/kernels/depthwise_conv_common.cc \
tensorflow/lite/micro/kernels/dequantize.cc \
tensorflow/lite/micro/kernels/detection_postprocess.cc \
tensorflow/lite/micro/kernels/elementwise.cc \
tensorflow/lite/micro/kernels/elu.cc \
tensorflow/lite/micro/kernels/ethosu.cc \
tensorflow/lite/micro/kernels/exp.cc \
tensorflow/lite/micro/kernels/expand_dims.cc \
tensorflow/lite/micro/kernels/fill.cc \
tensorflow/lite/micro/kernels/floor.cc \
tensorflow/lite/micro/kernels/floor_div.cc \
tensorflow/lite/micro/kernels/floor_mod.cc \
tensorflow/lite/micro/kernels/arc_mli/fully_connected.cc \
tensorflow/lite/micro/kernels/fully_connected_common.cc \
tensorflow/lite/micro/kernels/gather.cc \
tensorflow/lite/micro/kernels/gather_nd.cc \
tensorflow/lite/micro/kernels/hard_swish.cc \
tensorflow/lite/micro/kernels/if.cc \
tensorflow/lite/micro/kernels/kernel_runner.cc \
tensorflow/lite/micro/kernels/kernel_util.cc \
tensorflow/lite/micro/kernels/l2norm.cc \
tensorflow/lite/micro/kernels/l2_pool_2d.cc \
tensorflow/lite/micro/kernels/leaky_relu.cc \
tensorflow/lite/micro/kernels/logical.cc \
tensorflow/lite/micro/kernels/logistic.cc \
tensorflow/lite/micro/kernels/log_softmax.cc \
tensorflow/lite/micro/kernels/maximum_minimum.cc \
tensorflow/lite/micro/kernels/mul.cc \
tensorflow/lite/micro/kernels/neg.cc \
tensorflow/lite/micro/kernels/pack.cc \
tensorflow/lite/micro/kernels/pad.cc \
tensorflow/lite/micro/kernels/arc_mli/pooling.cc \
tensorflow/lite/micro/kernels/prelu.cc \
tensorflow/lite/micro/kernels/quantize.cc \
tensorflow/lite/micro/kernels/quantize_common.cc \
tensorflow/lite/micro/kernels/reduce.cc \
tensorflow/lite/micro/kernels/reshape.cc \
tensorflow/lite/micro/kernels/resize_bilinear.cc \
tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc \
tensorflow/lite/micro/kernels/round.cc \
tensorflow/lite/micro/kernels/shape.cc \
tensorflow/lite/micro/kernels/softmax.cc \
tensorflow/lite/micro/kernels/softmax_common.cc \
tensorflow/lite/micro/kernels/space_to_batch_nd.cc \
tensorflow/lite/micro/kernels/split.cc \
tensorflow/lite/micro/kernels/split_v.cc \
tensorflow/lite/micro/kernels/squeeze.cc \
tensorflow/lite/micro/kernels/strided_slice.cc \
tensorflow/lite/micro/kernels/sub.cc \
tensorflow/lite/micro/kernels/svdf.cc \
tensorflow/lite/micro/kernels/svdf_common.cc \
tensorflow/lite/micro/kernels/tanh.cc \
tensorflow/lite/micro/kernels/transpose.cc \
tensorflow/lite/micro/kernels/transpose_conv.cc \
tensorflow/lite/micro/kernels/unpack.cc \
tensorflow/lite/micro/kernels/zeros_like.cc 

LORA_SRCS := \
scenario_app/co2_person_lora_example/main.cc \
scenario_app/co2_person_lora_example/main_functions.cc \
tensorflow/lite/micro/kernels/arc_mli/scratch_buffers.cc \
tensorflow/lite/micro/kernels/arc_mli/scratch_buf_mgr.cc \
tensorflow/lite/micro/kernels/arc_mli/mli_slicers.cc \
tensorflow/lite/micro/examples/person_detection/himax_we1_evb/image_provider.cc \
tensorflow/lite/micro/examples/person_detection/model_settings.cc \
tensorflow/lite/micro/tools/make/downloads/person_model_int8/person_detect_model_data.cc

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

TFOBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(TFSRCS)))

qwiic_ccs811_bme280_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ccs811_bme280_SRCS)))

qwiic_ms8607_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ms8607_SRCS)))

qwiic_ms8607_sparkfun_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ms8607_sparkfun_SRCS)))

qwiic_ccs811_bme280_sparkfun_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(qwiic_ccs811_bme280_sparkfun_SRCS)))

LORA_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(LORA_SRCS)))

#=============================================================
# Applications settings
#=============================================================

MLI_PATH = third_party/arc_mli_package/bin/himax_arcem9d_r16/release
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
-I./third_party/gemmlowp \
-I./third_party/flatbuffers/include \
-I./third_party/ruy \
-I./third_party/arc_mli_package/include \
-I./third_party/arc_mli_package/include/api \
-I./tensorflow/lite/micro/tools/make/downloads/kissfft


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
-I./third_party/gemmlowp \
-I./third_party/flatbuffers/include \
-I./third_party/ruy \
-I./third_party/arc_mli_package/include \
-I./third_party/arc_mli_package/include/api \
-I./tensorflow/lite/micro/tools/make/downloads/kissfft

LDFLAGS+= \
$(MLI_PATH)/libmli.a \
-Hheap=8192 \
-Hnocopyr \
-m \
-Hldopt=-Coutput=$(MAP_NAME).map \
memory.lcf \
-Hldopt=-Bgrouplib \
$(SDK_PATH)/libboard_open_socket.a \
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
-fpermissive \
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
-I./third_party/gemmlowp \
-I./third_party/flatbuffers/include \
-I./third_party/ruy \
-I./third_party/arc_mli_package/include \
-I./third_party/arc_mli_package/include/api \
-I./tensorflow/lite/micro/tools/make/downloads/kissfft \
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
-I./third_party/gemmlowp \
-I./third_party/flatbuffers/include \
-I./third_party/ruy \
-I./third_party/arc_mli_package/include \
-I./third_party/arc_mli_package/include/api \
-I./tensorflow/lite/micro/tools/make/downloads/kissfft \

LDFLAGS +=  -Wl,-lmli -Wl,-lmwdepend -Wl,-marcv2elfx -Wl,-Map=memory.map -Wl,--strip-debug -Wl,--stats,--gc-sections -Wl,--cref \
-L$(MLI_PATH) \
-L$(DEPEND_PATH) \
-L. \
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

qwiic_ccs811_bme280.elf : $(qwiic_ccs811_bme280_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(qwiic_ccs811_bme280_OBJS) $(LDFLAGS)

qwiic_ms8607: MAP_NAME = qwiic_ms8607
qwiic_ms8607: qwiic_ms8607.elf

qwiic_ms8607.elf : $(qwiic_ms8607_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(qwiic_ms8607_OBJS) $(LDFLAGS)

qwiic_ccs811_bme280_sparkfun: MAP_NAME = qwiic_ccs811_bme280_sparkfun
qwiic_ccs811_bme280_sparkfun: qwiic_ccs811_bme280_sparkfun.elf

qwiic_ccs811_bme280_sparkfun.elf : $(qwiic_ccs811_bme280_sparkfun_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(qwiic_ccs811_bme280_sparkfun_OBJS) $(LDFLAGS)

qwiic_ms8607_sparkfun: MAP_NAME = qwiic_ms8607_sparkfun
qwiic_ms8607_sparkfun: qwiic_ms8607_sparkfun.elf

qwiic_ms8607_sparkfun.elf : $(qwiic_ms8607_sparkfun_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(qwiic_ms8607_sparkfun_OBJS) $(LDFLAGS)

co2_person_lora_example: MAP_NAME = co2_person_lora_example
co2_person_lora_example: co2_person_lora_example.elf

co2_person_lora_example.elf : $(LORA_OBJS) ${TFOBJS}
	$(CXX) $(CXXFLAGS) -o $@ $(LORA_OBJS) ${TFOBJS} $(LDFLAGS)

clean:
	@echo 'cleaning'
	-@$(RM) $(qwiic_ccs811_bme280_OBJS) $(qwiic_ms8607_OBJS) ${qwiic_ms8607_sparkfun_OBJS} ${qwiic_ccs811_bme280_sparkfun_OBJS} $(TFOBJS) $(LORA_OBJS)
	-@$(RM) *.elf
	-@$(RM) *.map
	
download: 
	@echo 'downloading'
	@$(DL) $(SDK_LINK)  -o $(SDK_NAME)
	@$(DL) $(TOOL_LINK)  -o $(TOOL_NAME)
	@$(DL) $(DEPEND_LINK)  -o $(DEPEND_NAME)
	@$(DL) $(TF_LINK) -o $(TF_NAME)
	@$(UNZIP) -o $(SDK_NAME) -d $(SDK_LOC) 
	@$(UNZIP) -o $(TOOL_NAME) -d $(TOOL_LOC)	
	@$(UNZIP) -o $(DEPEND_NAME) -d $(DEPEND_LOC)
	@$(UNZIP) -o $(TF_NAME) -d $(TF_LOC)
	@$(RM) $(SDK_NAME)
	@$(RM) $(TOOL_NAME)	
	@$(RM) $(DEPEND_NAME)
	@$(RM) $(TF_NAME)
	@$(CP) "himax_tflm-master/tensorflow/" "./"
	@$(RM) "himax_tflm-master"
	@mkdir -p $(MODEL_LOC)
	@$(DL) $(MODEL_LINK)  -o $(MODEL_NAME)
	@$(UNZIP) -o $(MODEL_NAME) -d $(MODEL_LOC)
	@$(RM) $(MODEL_NAME)
	@$(DL) $(LIB_LINK)  -o $(LIB_NAME)  
	@$(UNZIP) -o $(LIB_NAME) -d $(LIB_LOC)
	@$(RM) $(LIB_NAME)
	

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
