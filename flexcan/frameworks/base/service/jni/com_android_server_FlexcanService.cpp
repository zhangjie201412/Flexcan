#define LOG_TAG "FlexcanService"
#include <jni.h>
#include <JNIHelp.h>
#include "android_runtime/AndroidRuntime.h"
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/flexcan.h>
#include <stdio.h>

namespace android
{
	struct flexcan_device_t* flexcan_device = NULL;

	static jint flexcan_native_send(JNIEnv *env, jobject clazz,jintArray data, 
			jint id, jint dlc, jint extended,jint rtr, jint infinite, jint loopcount)
	{
		int BUF_SIZE = int(dlc);
		jint i;
		jint buf[BUF_SIZE];
		LOGI("Flexcan JNI: native send!!");
		env->GetIntArrayRegion(data, 0 ,8, buf);
//		for(i=0;i<BUF_SIZE;i++)
//			LOGI("Flexcan JNI: buf[%d] = %d",(int)i,(int)(buf[i]));  
		flexcan_device->flexcan_send(flexcan_device, buf, (int)id, (int)dlc, (int)extended,
				(int)rtr, (int)infinite, (int)loopcount);
		return 0;
	}

	static jobject flexcan_native_dump(JNIEnv *env, jobject clazz, jint id, jint mask, jobject frame)
	{
		jint data[8];
		jint can_id;
		jint dlc;
		int i;
		int ret;

		ret = flexcan_device->flexcan_dump(flexcan_device, (int)id, (int)mask);
		if(ret == 1)
			return NULL;
	//	data = flexcan_device->data;
		can_id = flexcan_device->can_id;
		dlc = flexcan_device->dlc;
	//	LOGE("Flexcan JNI: can_id: %d,dlc: %d",can_id,dlc);
		for(i=0;i<dlc;i++)
		{
			data[i] = (jint)flexcan_device->data[i];
		}
		//find class "com/android/server/FlexcanService"
		jclass frame_cls = env->FindClass("com/android/server/Frame");
		if(frame_cls == NULL) {
			LOGE("Flexcan JNI: find class FlexcanService error!!");
			return NULL;
		}

		//getm method id
		jmethodID setID = env->GetMethodID(frame_cls,
				"setID","(I)V");
		if( setID == NULL) {
			LOGE("Flexcan JNI: setID error!!");
			return NULL;
		}
		jmethodID setBuf = env->GetMethodID(frame_cls,
				"setBuf","([I)V");
		if(setBuf ==NULL){
			LOGE("Flexcan JNI: setBuf error!!");
			return NULL;
		}
		///////////////////////////////////
		jobject myFrame = frame;
		if(myFrame ==NULL) {
			LOGE("Flexcan JNI: frame NULL error!!");
			return NULL;
		}
		env->CallVoidMethod(myFrame,setID,can_id);
		///////////////////do with array////////////////
		jintArray arr;
		arr = env->NewIntArray(8);
		if(arr == NULL) {
			LOGE("Flexcan JNI: arr init error!!");
			return NULL;
		}
		env->SetIntArrayRegion(arr,0,8,data);
		env->CallVoidMethod(myFrame, setBuf, arr);

		env->DeleteLocalRef(arr);
		return myFrame;
	}

	//open device
	static inline int flexcan_device_open(const hw_module_t* module,struct flexcan_device_t** device)
	{
		return module->methods->open(module,"flexcan",(struct hw_device_t**)device);
	}

	static jboolean flexcan_init(JNIEnv* env,jclass clazz)
	{
		flexcan_module_t* module;

		LOGI("Flexcan JNI: initializing......");
		if(hw_get_module(FLEXCAN_HARDWARE_MODULE_ID,(const struct hw_module_t**)&module)==0)
		{
			LOGI("Flexcan JNI: flexcan stub found!");
			if(flexcan_device_open(&(module->common),&flexcan_device)==0)
			{
				LOGI("Flexcan JNI: flexcan device is open! ");
				return 0;
			}
			LOGE("Flexcan JNI: failed to open flexcan device!");
			return -1;
		}
		LOGE("Flexcan JNI: failed to get flexcan stub module!");
		return -1;
	}

	//JNI mehod table
	static const JNINativeMethod method_table[]=
	{
		{"init_native","()Z",(void*)flexcan_init},
		{"flexcan_native_send","([IIIIIII)I",(void*)flexcan_native_send},
		{"flexcan_native_dump","(IILcom/android/server/Frame;)Lcom/android/server/Frame;",(void*)flexcan_native_dump},
	};

	//register JNI method
	int register_android_server_FlexcanService(JNIEnv* env)
	{
		return jniRegisterNativeMethods(env,"com/android/server/FlexcanService",method_table,NELEM(method_table));
	}
}
