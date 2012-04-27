LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := cansend.c

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_MODULE := cansend

LOCAL_SYSTEM_SHARED_LIBRARIES := \
	libc libm libdl

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/../../kernel_imx/include/

LOCAL_CFLAGS := -O2 -g -W -Wall \
    -DHAVE_LINUX_FD_H 

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := candump.c

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_MODULE := candump

LOCAL_SYSTEM_SHARED_LIBRARIES := \
	libc libm libdl

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/../../kernel_imx/include/

LOCAL_CFLAGS := -O2 -g -W -Wall \
    -DHAVE_LINUX_FD_H 

include $(BUILD_EXECUTABLE)

