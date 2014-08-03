#ifndef _SRL_MONITOR_
#define _SRL_MONITOR_

#include "srl/sys/Thread.h"
#include "srl/rpc/RpcValue.h"

namespace SRL
{
    namespace Radmin
    {
        /**
        * Trigger
        * TODO explain purpose
        */
        class Trigger
        {
        public:
            enum Actions {
                STOP_MONITOR=1,
                REMOVE_TRIGGER=2,
                NOTIFY_INFO=4, 
                NOTIFY_WARN=8,
                NOTIFY_ERROR=16
            };
            
            enum Checks {
                STARTED=0,
                STOPPED,
                OUTPUT,
                GREATER_THAN,
                LESS_THAN,
                EQUAL_TO
            };
            
            Trigger(const String& name, const String& type, int actions, int checks) : 
                _name(name), _type(type), _actions(actions), _checks(checks){}

            /** default destructor */
            virtual ~Trigger(){};

            /** returns the name of the trigger */
            const String& name() const { return _name;}
            const String& type() const { return _type;}
            const int& actions() const { return _actions; }
            const int& checks() const { return _checks; }
            
            // TODO add asRpcValue
            
        protected:
            String _name, _type;
            int _actions;
            int _checks;
        };
        
        
        /**
        * KeywordTrigger
        * TODO explain purpose
        */
        class KeywordTrigger : public Trigger
        {
        public:
            /** default contstructor */
            KeywordTrigger(const String& name, const String& keyword, int actions) :
                Trigger(name, "keyword", actions, Trigger::EQUAL_TO), _keyword(keyword), _match(){}
            /** default destructor */
            virtual ~KeywordTrigger(){}
            
            bool check(const String& data) const;
            
            const String& keyword() const {return _keyword; }
            const String& match() const {return _match; }
                        
        protected:
            String _keyword;
            mutable String _match;
            
        };
        
        /**
        * ValueTrigger : public Trigger
        * TODO explain purpose
        */
        class ValueTrigger : public Trigger
        {
        public:
            /** int trigger contstructor */
            ValueTrigger(const String& name, int actions, int checks, 
                int value_id, int value) : 
                Trigger(name, "value", actions, checks), _value_id(value_id), 
                _int_value(value), _float_value(static_cast<double>(value)){}            

            /** float trigger contstructor */
            ValueTrigger(const String& name, int actions, int checks, 
                int value_id, double value) : 
                Trigger(name, "value", actions, checks), _value_id(value_id),
               _int_value(static_cast<int>(value)), _float_value(value){}
            /** default destructor_value */
            virtual ~ValueTrigger(){}
            
            const int& valueID() const { return _value_id; }
            const int& asInt() const { return _int_value; }
            const double& asDouble() const { return _float_value; }
            
            bool check(const int& data) const;
            bool check(const uint32& data) const;
            bool check(const float& data) const;            
            bool check(const double& data) const;
        protected:
            int _value_id;
            int _int_value;
            double _float_value;
        };
        
        
        /**
        * Monitor
        * Base Class for Monitors
        */
        class Monitor : public System::Thread
        {
        public:            
            class Listener
            {
            public:
                virtual ~Listener(){}       
                /** fired by the trigger */
                virtual void monitor_triggered(Monitor &monitor, Trigger &trigger)=0;
                virtual void monitor_error(Monitor &monitor, SRL::Errors::Exception &e)=0;
                virtual void monitor_stopped(Monitor &monitor)=0;
            };            
            /** default contstructor */
            Monitor(const String& name, const String& mtype);
            /** default destructor */
            virtual ~Monitor();
            
            /** returns the name of this monitor */
            const String& name() const {return _name;}
            /** returns the monitor type */
            const String& type() const {return _type;}

            /** returns an rpc struct with the current info for the monitored process, caller is responsible for cleanup */
            virtual RPC::RpcStructValue* asRpcValue() const;
            /** add a listener for the monitor triggers */
            void addListener(Monitor::Listener* listener) { _listeners.add(listener); }
            
            /** add a generic trigger to this monitor */
            void addTrigger(const String& name, int actions, int checks);
            void addTrigger(const String& name, const String& keyword, int actions);
            void addTrigger(const String& name, int actions, int checks, 
                int value_id, int value);
            void addTrigger(const String& name, int actions, int checks, 
                int value_id, double value);
        
        protected:
            virtual void final();
            void triggered(Trigger& trigger);
            void monitor_error(SRL::Errors::Exception &e);          
            void thread_exception(SRL::Errors::Exception &e);
            
            bool checkKeywords(const String& data);
            bool checkValues(const int& value_id, const int& value);
            bool checkValues(const int& value_id, const uint32& value);
            bool checkValues(const int& value_id, const float& value);
            bool checkValues(const int& value_id, const double& value);
        protected:
            String _name, _type;
            Util::Vector<Trigger*> _basic_triggers;
            Util::Vector<ValueTrigger*> _val_triggers;
            Util::Vector<KeywordTrigger*> _key_triggers;
            Util::Vector<Monitor::Listener*> _listeners;
        };
        
    } /* App */
    
} /* SRL */


#endif /* _SRL_MONITOR_ */
