
#include "JSystemInfo.h"
#include "srl/sys/ProcessList.h"
#include "srl/sys/SystemInfo.h"
#include "srl/net/NetworkInterface.h"
#include "srl/fs/File.h"

#include "srl/Console.h"
using namespace SRL;
// Get SystemInfo Singleton
System::SystemInfo *_sysinfo = NULL;
System::ProcessList *_proclist = NULL;

#define JNI_STRING "Ljava/lang/String;"

void JNICALL Java_jsrl_Version__1init(JNIEnv *env, jclass jcls)
{
	int major, minor, revision;
	SRL::GetVersion(major, minor, revision);

	jfieldID fid = env->GetStaticFieldID(jcls,"MAJOR", "I");
	env->SetStaticIntField(jcls, fid, major);
	
	fid = env->GetStaticFieldID(jcls,"MINOR", "I");
	env->SetStaticIntField(jcls, fid, minor);
	
	fid = env->GetStaticFieldID(jcls,"REVISION", "I");
	env->SetStaticIntField(jcls, fid, revision);
}


/** JSystemInfo._getCpuCount() */
jint JNICALL Java_jsrl_sys_SystemInfo__1getCpuCount(JNIEnv *env, jobject obj)
{
	if (_sysinfo == NULL)
	{
		_sysinfo = System::SystemInfo::GetInstance();
	}
	return (jint)_sysinfo->cpuCount();
}

void update_cpu(JNIEnv *env, jobject jcpu, const System::SystemInfo::CPU &cpu, bool all=false)
{
	jclass cls;
	jfieldID fid;
	jmethodID mid;
	
	cls = env->GetObjectClass(jcpu);
	if (all)
	{
		fid = env->GetFieldID(cls,"_vendorId", JNI_STRING);
		jstring jstr = env->NewStringUTF(cpu.vendor().text());
		env->SetObjectField(jcpu, fid, jstr);
		
		fid = env->GetFieldID(cls,"_model_name", JNI_STRING);
		jstr = env->NewStringUTF(cpu.modelName().text());
		env->SetObjectField(jcpu, fid, jstr);
		
		fid = env->GetFieldID(cls,"_family", "I");
		env->SetIntField(jcpu, fid, cpu.family());

		fid = env->GetFieldID(cls,"_model", "I");
		env->SetIntField(jcpu, fid, cpu.model());
		
		fid = env->GetFieldID(cls,"_type", "I");
		env->SetIntField(jcpu, fid, cpu.type());
				
	}
	
	fid = env->GetFieldID(cls, "_number", "I");
	env->SetIntField(jcpu, fid, cpu.number());

	fid = env->GetFieldID(cls, "_speed", "F");
	env->SetFloatField(jcpu, fid, cpu.speed());
	
	fid = env->GetFieldID(cls, "_cache", "I");
	env->SetIntField(jcpu, fid, cpu.cache());
	
	fid = env->GetFieldID(cls, "_load", "F");
	env->SetFloatField(jcpu, fid, cpu.load());
	
	fid = env->GetFieldID(cls, "_kernelLoad", "F");
	env->SetFloatField(jcpu, fid, cpu.kernelLoad());

	fid = env->GetFieldID(cls, "_userLoad", "F");
	env->SetFloatField(jcpu, fid, cpu.userLoad());

	fid = env->GetFieldID(cls, "_userTime", "J");
	env->SetLongField(jcpu, fid, cpu.userTime());

	fid = env->GetFieldID(cls, "_kernelTime", "J");
	env->SetLongField(jcpu, fid, cpu.kernelTime());

	fid = env->GetFieldID(cls, "_idleTime", "J");
	env->SetLongField(jcpu, fid, cpu.idleTime());
}

void update_memory(JNIEnv *env, jobject jmem, const System::SystemInfo::Memory &mem)
{
	jclass cls;
	jfieldID fid;
	jmethodID mid;

	cls = env->GetObjectClass(jmem);
	
	fid = env->GetFieldID(cls, "_used", "I");
	env->SetIntField(jmem, fid, mem.used()/1024);

	fid = env->GetFieldID(cls, "_free", "I");
	env->SetIntField(jmem, fid, mem.free()/1024);
	
	fid = env->GetFieldID(cls, "_total", "I");
	env->SetIntField(jmem, fid, mem.total()/1024);
}

/** JSystemInfo._updateCpu(CPU cpu, flag) */
void JNICALL Java_jsrl_sys_SystemInfo__1updateCpu(JNIEnv *env, 
jobject obj, jint index, jobject jcpu, jboolean flag)
{
	update_cpu(env, jcpu, _sysinfo->getCPU(index), flag);
}

/** JSystemInfo._updateAvgCpu(CPU cpu) */
void JNICALL Java_jsrl_sys_SystemInfo__1updateAvg(JNIEnv *env, 
jobject obj, jobject jcpu)
{
	_sysinfo->refresh();
	update_cpu(env, jcpu, _sysinfo->cpu(), false);
}

/** JSystemInfo._updateMemory(Memory memory, Memory swap) */
void JNICALL Java_jsrl_sys_SystemInfo__1updateMemory(JNIEnv *env,
jobject obj, jobject jmemory, jobject jswap)
{
	// get the cpu by index
	update_memory(env, jmemory, _sysinfo->memory());
	update_memory(env, jswap, _sysinfo->swap());
}

/** JSystemInfo._update(SystemInfo sys) */
void JNICALL Java_jsrl_sys_SystemInfo__1update(JNIEnv *env, 
jobject obj, jobject jsys, jobject jos)
{
	jclass cls;
	jfieldID fid;
	jmethodID mid;

	cls = env->GetObjectClass(jsys);
	
	fid = env->GetFieldID(cls,"_hostname", JNI_STRING);
	jstring jstr = env->NewStringUTF(_sysinfo->name().text());
	env->SetObjectField(jsys, fid, jstr);
	
	fid = env->GetFieldID(cls,"_domain", JNI_STRING);
	jstr = env->NewStringUTF(_sysinfo->domain().text());
	env->SetObjectField(jsys, fid, jstr);

	fid = env->GetFieldID(cls, "_logical_cpus", "I");
	env->SetIntField(jsys, fid, _sysinfo->numberOfLogicalCPUs());

	fid = env->GetFieldID(cls, "_physical_cpus", "I");
	env->SetIntField(jsys, fid, _sysinfo->numberOfPhysicalCPUs());
	
	fid = env->GetFieldID(cls, "_cores_per_cpu", "I");
	env->SetIntField(jsys, fid, _sysinfo->numberOfCoresPerCPU());

	fid = env->GetFieldID(cls, "_ht_supported", "Z");
	env->SetBooleanField(jsys, fid, _sysinfo->isHyperThreadSupported());
    	
	fid = env->GetFieldID(cls, "_ht_enabled", "Z");
	env->SetBooleanField(jsys, fid, _sysinfo->isHyperThreadEnabled());



	cls = env->GetObjectClass(jos);
	
	fid = env->GetFieldID(cls,"_name", JNI_STRING);
	jstr = env->NewStringUTF(_sysinfo->os().name().text());
	env->SetObjectField(jos, fid, jstr);
	
	fid = env->GetFieldID(cls,"_version", JNI_STRING);
	jstr = env->NewStringUTF(_sysinfo->os().version().text());
	env->SetObjectField(jos, fid, jstr);

	fid = env->GetFieldID(cls,"_kernel", JNI_STRING);
	jstr = env->NewStringUTF(_sysinfo->os().kernel().text());
	env->SetObjectField(jos, fid, jstr);

	fid = env->GetFieldID(cls, "_machine", JNI_STRING);
	jstr = env->NewStringUTF(_sysinfo->os().machine().text());
	env->SetObjectField(jos, fid, jstr);
}



/**
			
_update_process(int pid, int threads, int handles, int priority,  IIII
			float load, float kernel_load, float user_load, int cpuNumber, FFFI
			int size, int used, int faults) III
*/

void update_proc(JNIEnv *env, jobject jproclist, System::ProcessList::Process *proc=NULL, int pid=-1)
{
	if (proc == NULL)
	{
		proc = _proclist->getProcess(pid);
		if (proc == NULL)
			return;
	}
	
	jclass cls = env->GetObjectClass(jproclist);
	jmethodID mid = env->GetMethodID(cls, "_update_process", "(IIIIFFFIIIIJJ)V");
	if (mid == 0)
	{
		return;
	}
	env->CallVoidMethod(jproclist, mid, 
		    proc->id(), proc->threads(), proc->handles(), proc->priority(),
			proc->cpu().load(), proc->cpu().kernelLoad(), proc->cpu().userLoad(), 
			proc->cpu().cpuNumber(), proc->memory().virtualSize(), proc->memory().residentSize(), 
			proc->memory().faults(), proc->cpu().kernelTime(), proc->cpu().userTime());

}

/** _new_process(int pid, String name, String owner, long created) */
void new_proc(JNIEnv *env, jobject jproclist, int pid)
{
	System::ProcessList::Process *proc = _proclist->getProcess(pid);
	
	jclass cls = env->GetObjectClass(jproclist);
	jstring jstr_name = env->NewStringUTF(proc->name().text());
	jstring jstr_owner= env->NewStringUTF(proc->owner().text());
	jstring jstr_filename = env->NewStringUTF(proc->filename().text());
	jstring jstr_command= env->NewStringUTF(proc->command().text());
	
	jmethodID mid = env->GetMethodID(cls, "_new_process", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (mid == 0)
	{
		return;
	}	
	
	env->CallVoidMethod(jproclist, mid, proc->id(), proc->parentId(), jstr_name, jstr_owner, jstr_filename, jstr_command);
}

/** _dead_process(int pid) */
void dead_proc(JNIEnv *env, jobject jproclist, int pid)
{
	jclass cls = env->GetObjectClass(jproclist);
	jmethodID mid = env->GetMethodID(cls, "_dead_process", "(I)V");
	env->CallVoidMethod(jproclist, mid, pid);
}


/**

Signature   Java Type
  Z   boolean 
  B   byte
  C   char
  S   short
  I   int
  J   long
  F   float
  D   double
   L fully-qualified-class ;    fully-qualified-class 
  [ type   type[]
  ( arg-types ) ret-type   method type
  */

void proclist_update(JNIEnv *env, jobject jproclist, bool do_update=true)
{
	//SRL::TextFile *log = FS::OpenTextFile("jsrl_error.log", true);

	if (do_update)
	{
		_proclist->refresh();
	}
	// add all new procs
	Util::Vector<uint32>& new_pids = _proclist->newPids();	
	int size = new_pids.size();
	for (int i = 0; i < size; ++i)
	{
		new_proc(env, jproclist, new_pids[i]);
	}
	// update all procs
	System::ProcessList::Iterator iter = _proclist->begin();
	while (iter != _proclist->end())
	{
		update_proc(env, jproclist, iter.value());
		++iter;
	}
	// check for dead procs
	const Util::Vector<uint32>& dead_pids = _proclist->deadPids();
	size = dead_pids.size();
	for (int i = 0; i < size; ++i)
	{
		dead_proc(env, jproclist, dead_pids[i]);
	}
}

/** ProcessList._init */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList__1init(JNIEnv *env, jobject jproclist)
{
	if (_proclist == NULL)
	{
		_proclist = System::ProcessList::GetInstance();
	}
	proclist_update(env, jproclist, false);
}

/** ProcessList._end() */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList_end(JNIEnv *, jobject)
{
	//ians: i am removing this because of the CPOF RAT code producing a lockup that
	// could be here but I am not really sure
	//System::ProcessList::FreeInstance();
	//_proclist = NULL;
}

/** ProcessList._update() */
JNIEXPORT void JNICALL Java_jsrl_sys_ProcessList__1update(JNIEnv *env, jobject jproclist)
{
	proclist_update(env, jproclist);
}

/** ProcessList._updateProc(pid) */
jboolean JNICALL Java_jsrl_sys_ProcessList__1updateProc(JNIEnv *env, jobject jproclist, jint pid)
{
	update_proc(env, jproclist, NULL, pid);
	return true;
}

jboolean JNICALL Java_jsrl_sys_ProcessList_KillProcess(JNIEnv *env, jobject obj, jint pid)
{
	if (System::Kill((uint64)pid, true))
		return JNI_TRUE;
	return JNI_FALSE;
}



/** NetworkInterface._update() */
jboolean JNICALL Java_jsrl_net_NetworkInterface__1update(JNIEnv *env, jclass cls)
{
	Net::NetworkInterfaces *list = Net::NetworkInterface::GetAll();

	jstring jstr;
	jmethodID mid = env->GetStaticMethodID(cls, "_new_interface", "(Ljava/lang/String;)V");
	
	Net::NetworkInterfaces::Iterator iter = list->begin();
	for (; iter != list->end(); ++iter)
	{
		jstr= env->NewStringUTF(iter.value()->name().text());
		env->CallStaticVoidMethod(cls, mid, jstr);
	}
	return JNI_TRUE;
}

/** NetworkInterface._update(NetworkInterface iface) */
jboolean JNICALL Java_jsrl_net_NetworkInterface__1update_1iface(JNIEnv *env, jclass jmeth, jobject jiface)
{

	jclass cls = env->GetObjectClass(jiface);
	jfieldID fid = env->GetFieldID(cls,"_name", JNI_STRING);
	jstring jstr_name = (jstring)env->GetObjectField(jiface, fid);
	
	const char* name = env->GetStringUTFChars(jstr_name, 0);
	
	Net::NetworkInterface *iface = Net::NetworkInterface::GetByName(name);
	
	if (iface != NULL)
	{
		fid = env->GetFieldID(cls,"_ip", JNI_STRING);
		jstring jstr = env->NewStringUTF(iface->ip().text());
		env->SetObjectField(jiface, fid, jstr);
		
		fid = env->GetFieldID(cls,"_mac", JNI_STRING);
		jstr = env->NewStringUTF(iface->mac().text());
		env->SetObjectField(jiface, fid, jstr);	
		
		fid = env->GetFieldID(cls,"_is_up", "Z");
		env->SetBooleanField(jiface, fid, iface->isUp());	
		
		if (iface->isLoopback())
		{
			fid = env->GetFieldID(cls,"_is_loopback", "Z");
			env->SetBooleanField(jiface, fid, JNI_TRUE);
		}

		if (iface->isWifi())
		{
			fid = env->GetFieldID(cls,"_ap_mac", JNI_STRING);
			jstr = env->NewStringUTF(iface->apMac().text());
			env->SetObjectField(jiface, fid, jstr);	
			
			fid = env->GetFieldID(cls,"_essid", JNI_STRING);
			jstr = env->NewStringUTF(iface->essid().text());
			env->SetObjectField(jiface, fid, jstr);
			
			fid = env->GetFieldID(cls,"_is_wifi", "Z");
			env->SetBooleanField(jiface, fid, JNI_TRUE);
		}
        
        //printf("jni rx: %d\t", iface->rx().bytes());
 		jmethodID mid = env->GetMethodID(cls, "_update_rx", "(JJJJJ)V");
		env->CallVoidMethod(jiface, mid, iface->rx().bytes(), 
			iface->rx().packets(), iface->rx().errors(), 
			iface->rx().dropped(), iface->rx().multicast());
			
 		mid = env->GetMethodID(cls, "_update_tx", "(JJJJ)V");
		env->CallVoidMethod(jiface, mid, iface->tx().bytes(), 
			iface->tx().packets(), iface->tx().errors(), 
			iface->tx().dropped());
		
	}
	
	env->ReleaseStringUTFChars(jstr_name, name);
	return JNI_TRUE;
}



