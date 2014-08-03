
#include "JavaPingImpl.h"
#include "srl/net/IcmpSocket.h"

#ifdef WIN32
#include "srl/net/icmpdll.h"
#endif

using namespace SRL;
using namespace SRL::Net;
// create out local ping
ICMP::IcmpSocket *_ping = NULL;
/*
 * Class:     Java_com_giti_net_JavaPing_begin
 * Method:    begin
 * Signature: ()V
 */
jboolean JNICALL Java_jsrl_net_JavaPing_begin(JNIEnv *env, jobject obj)
{
	try
	{
		if (_ping == NULL)
			_ping = new ICMP::IcmpSocket();
		return JNI_TRUE;
	}
	catch (...)
	{
		//std::cout << "permission denied for raw sockets" << std::endl;
	}
	return JNI_FALSE;
}

/*
 * Class:     Java_com_giti_net_JavaPing_end
 * Method:    end
 * Signature: ()V
 */
void JNICALL Java_jsrl_net_JavaPing_end(JNIEnv *env, jobject obj)
{
		if (_ping != NULL)
		{
			delete _ping;
			_ping = NULL;
		}
}

/*
 * Class:     Java_com_giti_net_JavaPing_ping
 * Method:    ping
 * Signature: (Ljava/lang/String;II)V
 */
jboolean JNICALL Java_jsrl_net_JavaPing_ping
  (JNIEnv *env, jobject object, jstring address, jint seq, jint msg_size, jint ttl)
{
	if (_ping == NULL)
		return false;
	
	try
	{
		const char* cstr = env->GetStringUTFChars(address, 0);
		String ip = cstr;
		_ping->sendEchoRequest(ip,seq,msg_size, ttl);
		env->ReleaseStringUTFChars(address, cstr);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*
 * Class:     Java_com_giti_net_JavaPing__1pong
 * Method:    _pong
 * Signature: (ILcom/giti/net/PingResult;)V
 */
jboolean JNICALL Java_jsrl_net_JavaPing__1pong
  (JNIEnv *env, jobject obj, jint time_out, jobject jresult)
{
	ICMP::Response reply;
	try
	{
		// attempt to get data
		reply = _ping->receiveEchoReply(time_out);
	}
	catch(...)
	{
		return false;
	}

	jclass cls;
	jfieldID fid;
	jmethodID mid;

	cls = env->GetObjectClass(jresult);
	fid = env->GetFieldID(cls,"from", "Ljava/lang/String;");
	jstring jstr = env->NewStringUTF(reply.from().getAddress().getHostAddress().text());
	env->SetObjectField(jresult, fid, jstr);

	fid = env->GetFieldID(cls, "rtt", "F");
	env->SetFloatField(jresult, fid, reply.roundTripTime());
	
	fid = env->GetFieldID(cls, "ttl", "I");
	env->SetIntField(jresult, fid, reply.timeToLive());

	fid = env->GetFieldID(cls, "bytes", "I");
	env->SetIntField(jresult, fid, reply.size());

	fid = env->GetFieldID(cls, "seq", "I");
	env->SetIntField(jresult, fid, reply.sequence());	

	fid = env->GetFieldID(cls, "type", "I");
	env->SetIntField(jresult, fid, reply.type());
	
	return true;
}

/*
 * Class:     Java_com_giti_net_JavaPing_pingpong
 * Method:    pingpong
 * Signature: (Ljava/lang/String;II)V
 */
jboolean JNICALL Java_jsrl_net_JavaPing__1pingpong
  (JNIEnv *env, jobject object, jstring address, jint seq, jint msg_size, jint time_out, jint ttl, jobject jresult)
{
#ifdef WIN32

	ICMP::Response reply;
	
	try
	{
		const char* cstr = env->GetStringUTFChars(address, 0);
		String ip = cstr;
		reply = SRL::Net::ICMP::IcmpDllEchoRequest(ip, seq, msg_size, time_out, ttl);
		env->ReleaseStringUTFChars(address, cstr);
	}
	catch(...)
	{
		return false;
	}

	jclass cls;
	jfieldID fid;
	jmethodID mid;

	cls = env->GetObjectClass(jresult);
	fid = env->GetFieldID(cls,"from", "Ljava/lang/String;");
	jstring jstr = env->NewStringUTF(reply.from().getAddress().getHostAddress().text());
	env->SetObjectField(jresult, fid, jstr);

	fid = env->GetFieldID(cls, "rtt", "F");
	env->SetFloatField(jresult, fid, reply.roundTripTime());
	
	fid = env->GetFieldID(cls, "ttl", "I");
	env->SetIntField(jresult, fid, reply.timeToLive());

	fid = env->GetFieldID(cls, "bytes", "I");
	env->SetIntField(jresult, fid, reply.size());

	fid = env->GetFieldID(cls, "seq", "I");
	env->SetIntField(jresult, fid, reply.sequence());	

	fid = env->GetFieldID(cls, "type", "I");
	env->SetIntField(jresult, fid, reply.type());

	return true;

#else
	if (_ping == NULL)
		return false;

	if (!Java_jsrl_net_JavaPing_ping(env, object, address, seq, msg_size, ttl))
		return false;
	if (!Java_jsrl_net_JavaPing__1pong(env, object, time_out, jresult))
		return false;
	return true;
#endif
}