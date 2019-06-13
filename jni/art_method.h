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
 
#ifndef ART_METHOD_H_
#define ART_METHOD_H_
 
#include <jni.h>
#include <string>
 
#include "type.h"
#include "alog.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
typedef enum  {
	type_android_unkwon = 0, type_android4x, type_android5, type_androidl, type_android6, type_android7
} Android_Type;
 
 
// 为测试art method版本设计
typedef struct PACKED(4) {
	u4 padding_[20];
} ArtMethod;
 
#define ART_METHOD_INDEX_SIZE (sizeof(ArtMethod) / sizeof(u4))
 
typedef struct {
	u4 klass_;
	u4 monitor_;
	void* declaring_class_;
	void* dex_cache_initialized_static_storage_;
	void* dex_cache_resolved_methods_;
	void* dex_cache_resolved_types_;
	void* dex_cache_strings_;
	u4 access_flags_;
	u4 code_item_offset_;
	u4 core_spill_mask_;
	const void* entry_point_from_compiled_code_;
	void* entry_point_from_interpreter_;
	u4 fp_spill_mask_;
	size_t frame_size_in_bytes_;
	const u2* gc_map_;
	const u4* mapping_table_;
	u4 method_dex_index_;
	u4 method_index_;
	const void* native_method_;
	const u2* vmap_table_;
 
} ArtMethod4x;
 
typedef struct  {
	u4 klass_;
	u4 monitor_;
	u4 declaring_class_;
	u4 dex_cache_resolved_methods_;
	u4 dex_cache_resolved_types_;
	u4 dex_cache_strings_;
	u8 entry_point_from_interpreter_;
	u8 entry_point_from_jni_;
	u8 entry_point_from_quick_compiled_code_;
	u8 gc_map_;
	u4 access_flags_;
	u4 dex_code_item_offset_;
	u4 dex_method_index_;
	u4 method_index_;
} ArtMethod5;
 
typedef struct {
	u4 klass_;
	u4 monitor_;
	u4 declaring_class_;
	u4 dex_cache_resolved_methods_;
	u4 dex_cache_resolved_types_;
	u4 access_flags_;
	u4 dex_code_item_offset_;
	u4 dex_method_index_;
	u4 method_index_;
	void* entry_point_from_interpreter_;
	void* entry_point_from_jni_;
	void* entry_point_from_quick_compiled_code_;
} ArtMethodl;
 
typedef struct {
	u4 klass_;
	u4 monitor_;
	u4 declaring_class_;
	u4 dex_cache_resolved_methods_;
	u4 dex_cache_resolved_types_;
	u4 access_flags_;
	u4 dex_code_item_offset_;
	u4 dex_method_index_;
	u4 method_index_;
	void* entry_point_from_interpreter_;
	void* entry_point_from_jni_;
	void* entry_point_from_quick_compiled_code_;
} ArtMethod6;
 
typedef struct {
	u4 klass_;
	u4 monitor_;
	u4 declaring_class_;
	u4 access_flags_;
	u4 dex_code_item_offset_;
	u4 dex_method_index_;
	u2 method_index_;
	u2 hotness_count_;
	void** dex_cache_resolved_methods_;
	void* dex_cache_resolved_types_;
	void* entry_point_from_jni_;
	void* entry_point_from_quick_compiled_code_;
} ArtMethod7;
 
typedef struct {
	Android_Type android_type;
	int size;
} Method_Info;
 
static Method_Info minfo;
 
static Method_Info get_method_info() {
	return minfo;
}
 
//创建ArtMethod结构
void* create_method_struct();
 
//对应native 函数
void m0();
 
jboolean clone_method(JNIEnv* env, jclass cl, jobject ori, jobject dst);
 
void set_apilevel(JNIEnv* env, jclass cl, jint apil);
 
bool register_art_method(JNIEnv* env);
 
#ifdef TYPE_ALL
/**
 * 通过注册一个native函数，得到native函数地址，然后env得到该artmethod结构的地址
 * 通过对比那个地址值等于entry_point_from_jni_，得到偏移
 * @parma env, env
 * @return, <=0 failed > 0 success
 */
size_t get_offset_to_jni(JNIEnv* env);
 
//通过size获得art method类型，被设置method info
Android_Type get_method_type_by_size(size_t size);
 
/**
 * 通过注册一个native函数，得到native函数地址，然后env得到该artmethod结构的地址，通过对比那个地址值等于entry_point_from_jni_
 * 然后根据偏移得到artmethod类型
 * @param offset, 偏移
 * @return, 返回对应类型
 */
Android_Type get_method_type_by_off(size_t offset);
 
/**
 * 初始化android type
 */
bool init_android_info(JNIEnv* env);
#endif
 
#ifdef __cplusplus
};
#endif
 
#endif /* ART_METHOD_H_ */