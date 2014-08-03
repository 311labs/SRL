#include "JKeyboard.h"

#include "srl/sys/Keyboard.h"
#include "srl/Console.h"


using namespace SRL;
using namespace SRL::System;

// because windows gets a call back from the system on the keyevent we
// need to attach our thread to the jvm to perform the callback
jobject g_obj;
JavaVM *g_jvm = NULL;
jmethodID g_jpressed_id;
jmethodID g_jreleased_id;

class JavaKeyboard : public System::Keyboard
{
public:
	JavaKeyboard() : _jenv(NULL), System::Keyboard(false)
    {
		Thread::start();
		System::Sleep(100);
    }
protected:

    virtual bool initial()
    {
        // attach to the jvm
        g_jvm->AttachCurrentThread((void**)&_jenv, NULL);
        return Keyboard::initial();
    }
    
    virtual void final()
    {
        // release from the jvm
        g_jvm->DetachCurrentThread();
        Keyboard::final();
    }
    
    virtual void _hotkey_pressed(HotKey *hk)
    {
        _jenv->CallVoidMethod(g_obj, g_jpressed_id, hk->id);
		//Console::writeLine("HOTKEY PRESSED DONE");
    }
    
    virtual void _hotkey_released(HotKey *hk)
    {
        _jenv->CallVoidMethod(g_obj, g_jreleased_id, hk->id);
    }
protected:
    JNIEnv *_jenv;
};

JavaKeyboard *g_keyboard;

/** JKeyboard.start() */
void JNICALL Java_jsrl_sys_Keyboard_start(JNIEnv *env, jobject obj)
{
	if (g_keyboard == NULL)
	{
	    env->GetJavaVM(&g_jvm);
	    g_obj = env->NewGlobalRef(obj);
	    
	 	g_jpressed_id = env->GetMethodID(env->GetObjectClass(g_obj), "_hotkey_pressed", "(I)V");
    	g_jreleased_id = env->GetMethodID(env->GetObjectClass(g_obj), "_hotkey_released", "(I)V");   
	    
		g_keyboard = new JavaKeyboard();
	}
}

/** JKeyboard.stop() */
void JNICALL Java_jsrl_sys_Keyboard_stop(JNIEnv *env, jobject obj)
{
	if (g_keyboard != NULL)
	{
		delete g_keyboard;
	    env->DeleteGlobalRef(g_obj);
		g_keyboard = NULL;
	}
}

/** JKeyboard.registerHotKey() */
jint JNICALL Java_jsrl_sys_Keyboard__1registerHotKey(JNIEnv *env, jobject obj, jint modifiers, 
                                                    jint keycode, jboolean passthrough)
{
    return g_keyboard->registerHotkey(modifiers, keycode, passthrough);
}

/** JKeyboard.unregisterHotKey() */
void JNICALL Java_jsrl_sys_Keyboard__1unregisterHotKey(JNIEnv *env, jobject obj, jint id)
{
    g_keyboard->unregisterHotkey(id);
}


