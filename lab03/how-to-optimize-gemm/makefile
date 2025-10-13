OLD  := MMult_cuda0
NEW  := openblas

CC         := gcc
LINKER     := nvcc        #用 nvcc 链接，兼容 CUDA 对象文件

# CUDA 编译器与参数
CUDA_CC     := nvcc
CUDA_CFLAGS := -g -Xcompiler -fPIC -Wno-deprecated-gpu-targets -gencode arch=compute_89,code=sm_89
CUDA_LDFLAGS := -lcudart

# 通用 C 编译参数
CFLAGS     := -Wall -Werror -Wno-unused-result -Wno-unused-value -Wno-unused-variable -g -fdiagnostics-color=auto -pthread -fopenmp
LDFLAGS    := -lm -lopenblas $(CUDA_LDFLAGS) #-pthread -fopenmp 

DATA_DIR   = _data
BUILD_DIR  = _build

# 源文件对象（默认）
OBJS := $(BUILD_DIR)/util.o $(BUILD_DIR)/REF_MMult.o $(BUILD_DIR)/test_MMult.o $(BUILD_DIR)/$(NEW).o

RED = \033[1;31m
NC  = \033[0m   # No Color

$(shell mkdir -p $(BUILD_DIR) $(DATA_DIR))

# C
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo "+ CC $<"
	$(CC) -std=gnu11 $(CFLAGS) -c $< -o $@


# CUDA
$(BUILD_DIR)/%.o: %.cu
	@mkdir -p $(dir $@) && echo "+ NVCC $<"
	$(CUDA_CC) $(CUDA_CFLAGS) -c $< -o $@


all:
	make clean
	make $(BUILD_DIR)/test_MMult.x
$(BUILD_DIR)/test_MMult.x: $(OBJS) defs.h
	$(LINKER) $(OBJS) $(LDFLAGS) -o $@ || (echo "$(RED)\nFailed!!!  Please check the code and linking options.$(NC)"; exit 1)


run:
	make all
	echo $$OMP_NUM_THREADS
	@echo "date = '`date`';" > $(DATA_DIR)/output_$(NEW).m
	@echo "version = '$(NEW)';" >> $(DATA_DIR)/output_$(NEW).m
	$(BUILD_DIR)/test_MMult.x >> $(DATA_DIR)/output_$(NEW).m
	@if [ ! -f $(DATA_DIR)/output_old.m ] || [ $(OLD) != $(NEW) ]; then \
        cp $(DATA_DIR)/output_$(OLD).m $(DATA_DIR)/output_old.m; \
    fi
	cp $(DATA_DIR)/output_$(NEW).m $(DATA_DIR)/output_new.m


clean:
	rm -rf _build *.jpg

cleanall:
	rm -rf _build _data *.jpg *.png
