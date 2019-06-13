/**
 * @Atuthor : sp00f
 * @Version: 0.1
 * @Desc: art hook
 * @Theory: jave proxy, reflect
 * 实现方式其实有很多种，这种或许最简单
 * 另外检测是哪个版本也有很多种方式，如检测系统属性，检测apilevel
 * 这里随便写了个（art method结构体大小）仅供参考
 * 我本人之测过几部手机，对于兼容性不敢保证，此实现仅用于学习和讨论
 * 如果用于产品由于兼容性问题请自发解决（因为原理很简单）
 */
 
#include "art_method.h"
 
#include "config.h"
#include <jni.h>
 
#define PACKAGE_PREFIX "tj.spf" // TODO 你自己指定
 
__inline__ std::string get_class(const char* clazz) {
	std::string class_name(PACKAGE_PREFIX);
	class_name.append(clazz);
	return class_name;
}
 
static std::string class_name = get_class("ArtMethod");
static std::string art_method_clone("cloneMethod");
static std::string art_method_m0("m0");
static std::string art_method_set_apiLevel("setApiLevel");
 
static JNINativeMethod art_methods[] ={
		{ art_method_clone.c_str(), "(Ljava/lang/reflect/Method;Ljava/lang/reflect/Method;)Z", (void*) clone_method },
		{ art_method_m0.c_str(), "()V", (void*) m0 },
		{ art_method_set_apiLevel.c_str(), "(I)V", (void*) set_apilevel}
};
 
//此函数作用是修改非静态为静态，私有为共有，这么做的目的是防止底层检测，否则上层函数必须是静态的，要不然会崩溃
static void set_access_flags(void* method);
 
#ifdef TYPE_ALL
Android_Type get_method_type_by_size(size_t size) {
	size += 2 * sizeof(u4);
	switch(size) {
	case sizeof(ArtMethod5):
		minfo.android_type = type_android5;
		minfo.size = sizeof(ArtMethod5);
		return type_android5;
//	case sizeof(ArtMethodl):
//		minfo.android_type = type_androidl;
//		minfo.size = sizeof(ArtMethodl);
//		return type_androidl;
	case sizeof(ArtMethod6):
		minfo.android_type = type_android6;
		minfo.size = sizeof(ArtMethod6);
		return type_android6;
	default:
		minfo.android_type = type_android_unkwon;
		minfo.size = 0;
		return type_android_unkwon;
	}
}
 
Android_Type get_method_type_by_off(size_t offset) {
	if (offsetof(ArtMethod6, entry_point_from_jni_) == offset) {
		minfo.android_type = type_android6;
		minfo.size = sizeof(ArtMethod6);
		return type_android6;
	}
 
	minfo.android_type = type_android_unkwon;
	minfo.size = 0;
	return type_android_unkwon;
}
 
size_t get_offset_to_jni(JNIEnv* env) {
	size_t off = 0;
	jclass clazz = env->FindClass(class_name.c_str());
 
	if (!clazz) {
		ALOGE("[-] find class %s failed", class_name.c_str());
		return -1;
	}
 
	jmethodID art_m0 = env->GetStaticMethodID(clazz, art_method_m0.c_str(), "()V");
 
	if (!art_m0) {
		ALOGE("[-] find class %s method %s failed", class_name.c_str(), art_method_m0.c_str());
		return -1;
	}
 
	ArtMethod* art_method = (ArtMethod*) art_m0;
 
	size_t* p = (size_t*) art_method;
 
	ALOGI("[*] art method index size = %d", ART_METHOD_INDEX_SIZE);
 
	size_t native_method_addr = reinterpret_cast<size_t>((void*)m0);
 
	ALOGI("[*] native method addr = %p", m0);
 
	for (int i = 0; i < ART_METHOD_INDEX_SIZE; i++) {
		size_t rp = *p;
 
		ALOGI("[*] rp = %ul", rp);
 
		if (rp == native_method_addr) {
			off = i * (sizeof(u4));
			ALOGI("[*] off = %d", off);
		}
 
		p++;
	}
 
    env->DeleteLocalRef(clazz);
 
	return off;
 
}
 
bool init_android_info(JNIEnv* env) {
	ALOGI("[*] init art method info");
	bool r = false;
	jmethodID java_m1 = NULL;
	jmethodID java_m2 = NULL;
	size_t method_width = 0;
	Android_Type a_type = type_android_unkwon;
 
	jclass clazz = env->FindClass(class_name.c_str());
 
	if (!clazz) {
		ALOGE("[-] Error: Find class %s failed", class_name.c_str());
		goto exit;
	}
 
	java_m1 = env->GetMethodID(clazz, "m1", "()V");
	java_m2 = env->GetMethodID(clazz, "m2", "()V");
 
	if (!java_m1) {
		ALOGE("[-] Error: Find class %s method %s failed", class_name.c_str(), "m1");
		goto exit;
	}
 
	if (!java_m2) {
		ALOGE("[-] Error: Find class %s method %s failed", class_name.c_str(), "m2");
		goto exit;
	}
 
	ALOGI("[*] Find class and method success");
 
	method_width = reinterpret_cast<size_t>(java_m2) - reinterpret_cast<size_t>(java_m1);
 
	ALOGI("[*] simple method size = %u", method_width);
 
	a_type = get_method_type_by_size(method_width);
	if (a_type == type_android_unkwon) {
		ALOGE("[-] get art method type failed");
		goto exit;
	}
 
	ALOGI("[*] android type = %d, method size = %d", a_type, minfo.size);
 
	if (a_type != type_android_unkwon) {
		r = true;
		ALOGI("[*] init art method info success");
	}
 
	exit:
		if(clazz) {
			env->DeleteLocalRef(clazz);
		}
 
		return r;
}
 
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv*		env 		= NULL;
 
	ALOGI("[*] >>> Into JNI onload");
 
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
    	ALOGE("[-] Error: GetEnv failed");
    	return JNI_ERR;
    }
 
    if (!register_art_method(env)) {
    	return JNI_ERR;
    }
 
    return JNI_VERSION_1_4;
 
}
 
#endif
 
void* create_method_struct() {
	void* method = NULL;
	if (minfo.android_type == type_android5) {
		method = malloc(minfo.size);
	} else if (minfo.android_type == type_androidl) {
		method = malloc(minfo.size);
	} else if (minfo.android_type == type_android6) {
		method = malloc(minfo.size);
	}
 
	if (!method) {
		ALOGE("[-] alloc method struct failed");
		return NULL;
	}
 
	memset(method, 0, minfo.size);
 
	ALOGI("[*] alloc method struct");
 
	return method;
}
 
void set_apilevel(JNIEnv* env, jclass cl, jint apil) {
    if (apil > 23) {
        minfo.android_type = type_android7;
        minfo.size = sizeof(ArtMethod7);
    } else if (apil > 22) {
        minfo.android_type = type_android6;
        minfo.size = sizeof(ArtMethod6);
	} else if (apil > 21) {
        minfo.android_type = type_androidl;
        minfo.size = sizeof(ArtMethodl);
	} else if (apil > 19) {
        minfo.android_type = type_android5;
        minfo.size = sizeof(ArtMethod5);
    } else {
        minfo.android_type = type_android4x;
        minfo.size = sizeof(ArtMethod4x);
    }
}
 
jboolean clone_method(JNIEnv* env, jclass cl, jobject ori, jobject dst) {
	ALOGI("[*] clone art method");
 
	jboolean r = false;
	jmethodID origin = NULL;
	jmethodID dest    = NULL;
	void* dest_method = NULL;
	void* src_method = NULL;
 
	origin = env->FromReflectedMethod(ori);
	dest = env->FromReflectedMethod(dst);
 
	if (!origin) {
		ALOGE("[-] find class %s method origin failed", class_name.c_str());
		goto exit;
	}
 
	if (!dest) {
		ALOGE("[-] find class %s method back failed", class_name.c_str());
		goto exit;
	}
 
	set_access_flags(dest);
 
	src_method = reinterpret_cast<void*>(origin);
	dest_method = reinterpret_cast<void*>(dest);
 
	ALOGI("[*] clone art method cast method");
 
	if (minfo.android_type != type_android_unkwon && minfo.size > 0) {
		ALOGI("[*] clone art method backed");
		memcpy(src_method, dest_method, minfo.size);
		ALOGI("[*] clone art method success");
	}
 
	r = true;
 
	exit:
		return r;
 
}
 
static void set_access_flags(void* method) {
	// ACC_NATIVE = 0x00000100,       // method
	// ACC_PUBLIC = 0x00000001,       // class, field, method, ic
	if (minfo.android_type == type_android6) {
		ArtMethod6* art6 = (ArtMethod6*)method;
		art6->access_flags_ = art6->access_flags_ & (~0x0002) | 0x0001;
	}
}
 
void m0() {
	ALOGI("[*] simple method native impl");
}
 
bool register_art_method(JNIEnv* env) {
	ALOGI("[*] register art method class native method");
 
	jclass clazz = env->FindClass(class_name.c_str());
	bool r = false;
	if (!clazz) {
		ALOGE("[-] find class %s failed", class_name.c_str());
		goto exit;
	}
 
	if (env->RegisterNatives(clazz, art_methods, NELEM(art_methods)) < 0) {
		ALOGE("[-] register class %s method %s failed", class_name.c_str(), art_method_clone.c_str());
		goto exit;
	}
 
	r = true;
 
	ALOGI("[*] register art method class native method success");
 
	exit:
		if (clazz) {
			env->DeleteLocalRef(clazz);
		}
 
    	return r;
}