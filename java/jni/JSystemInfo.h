/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class jsrl_sys_ProcessList */

#ifndef _Included_jsrl_sys_ProcessList
#define _Included_jsrl_sys_ProcessList
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     jsrl_Version
 * Method:    _init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jsrl_Version__1init
  (JNIEnv *, jclass);

/*
 * Class:     jsrl_sys_SystemInfo
 * Method:    _getCpuCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jsrl_sys_SystemInfo__1getCpuCount
  (JNIEnv *, jobject);

/*
 * Class:     jsrl_sys_SystemInfo
 * Method:    _updateCpu
 * Signature: (ILjsrl/sys/SystemInfo/CPU;Z)V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_SystemInfo__1updateCpu
  (JNIEnv *, jobject, jint, jobject, jboolean);

/*
 * Class:     jsrl_sys_SystemInfo
 * Method:    _updateMemory
 * Signature: (Ljsrl/sys/SystemInfo/Memory;Ljsrl/sys/SystemInfo/Memory;)V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_SystemInfo__1updateMemory
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     jsrl_sys_SystemInfo
 * Method:    _update
 * Signature: (Ljsrl/sys/SystemInfo;Ljsrl/sys/SystemInfo/OS;)V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_SystemInfo__1update
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     jsrl_sys_SystemInfo
 * Method:    _updateAvg
 * Signature: (Ljsrl/sys/SystemInfo/CPU;)V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_SystemInfo__1updateAvg
  (JNIEnv *, jobject, jobject);
/*
 * Class:     jsrl_sys_ProcessList
 * Method:    KillProcess
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_jsrl_sys_ProcessList_KillProcess
  (JNIEnv *, jobject, jint);

/*
 * Class:     jsrl_sys_ProcessList
 * Method:    _init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList__1init
  (JNIEnv *, jobject);

/*
 * Class:     jsrl_sys_ProcessList
 * Method:    end
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList_end
  (JNIEnv *, jobject);

/*
 * Class:     jsrl_sys_ProcessList
 * Method:    _update
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList__1update
  (JNIEnv *, jobject);

/*
 * Class:     jsrl_sys_ProcessList
 * Method:    _updateProc
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_jsrl_sys_ProcessList__1updateProc
  (JNIEnv *, jobject, jint);


/*
 * Class:     jsrl_net_NetworkInterface
 * Method:    _update
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_jsrl_net_NetworkInterface__1update
  (JNIEnv *, jclass);

/*
 * Class:     jsrl_net_NetworkInterface
 * Method:    _update_iface
 * Signature: (Ljsrl/net/NetworkInterface;)Z
 */
JNIEXPORT jboolean JNICALL Java_jsrl_net_NetworkInterface__1update_1iface
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif
