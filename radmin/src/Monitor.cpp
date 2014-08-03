#include "srl/radmin/Monitor.h"

using namespace SRL;
using namespace SRL::Radmin;
using namespace SRL::System;

Monitor::Monitor(const String& name, const String& mtype) : Thread(THREAD_NORMAL, name), _name(name), _type(mtype)
{
    
}

Monitor::~Monitor()
{
    // free all the remaining triggers
    _basic_triggers.clear(true);
    _key_triggers.clear(true);
    _val_triggers.clear(true);
}

void Monitor::addTrigger(const String& name, int actions, int events)
{
    Trigger *trigger = new Trigger(name, "basic", actions, events);
    _basic_triggers.add(trigger);
}

void Monitor::addTrigger(const String& name, const String& keyword, int actions)
{
    KeywordTrigger *trigger = new KeywordTrigger(name, keyword, actions);
    _key_triggers.add(trigger);
}

void Monitor::addTrigger(const String& name, int actions, int events, 
    int value_id, int value)
{
    ValueTrigger *trigger = new ValueTrigger(name, actions, events, value_id, value);
    _val_triggers.add(trigger);
}
    
void Monitor::addTrigger(const String& name, int actions, int events, 
    int value_id, double value)
{
    ValueTrigger *trigger = new ValueTrigger(name, actions, events, value_id, value);
    _val_triggers.add(trigger);
}


void Monitor::final()
{
    _basic_triggers.clear(true);
    _key_triggers.clear(true);
    _val_triggers.clear(true);
    
    Util::Vector<Monitor::Listener*>::Iterator iter = _listeners.begin();
    for (; iter!=_listeners.end(); ++iter)
    {
        iter->monitor_stopped(*this);
    }
}

void Monitor::triggered(Trigger& trigger)
{
    Util::Vector<Monitor::Listener*>::Iterator iter = _listeners.begin();
    for (; iter!=_listeners.end(); ++iter)
    {
        iter->monitor_triggered(*this, trigger);
    }
}

void Monitor::thread_exception(SRL::Errors::Exception &e)
{
    monitor_error(e);
}

void Monitor::monitor_error(SRL::Errors::Exception &e)
{
    Util::Vector<Monitor::Listener*>::Iterator iter = _listeners.begin();
    for (; iter!=_listeners.end(); ++iter)
    {
        iter->monitor_error(*this, e);
    }
}

bool Monitor::checkKeywords(const String& data)
{
    Util::Vector<KeywordTrigger*>::Iterator iter = _key_triggers.begin();
    for (; iter!=_key_triggers.end(); ++iter)
    {
        if (iter->check(data))
        {
            triggered(*iter.value());
            if (iter->actions() & Trigger::STOP_MONITOR)
                return false;
            if (iter->actions() & Trigger::REMOVE_TRIGGER)
            {
                delete iter.value();
                iter = _key_triggers.erase(iter);
                --iter;
            }
        }
    }
    
    return true;
}

bool Monitor::checkValues(const int& value_id, const int& value)
{
    Util::Vector<ValueTrigger*>::Iterator iter = _val_triggers.begin();
    for (; iter!=_val_triggers.end(); ++iter)
    {
        if (iter->valueID() == value_id)
        {
            if (iter->check(value))
            {
                triggered(*iter.value());
                if (iter->actions() & Trigger::STOP_MONITOR)
                    return false;
                if (iter->actions() & Trigger::REMOVE_TRIGGER)
                {
                    iter = _val_triggers.erase(iter);
                    --iter;
                }
            }
        }
    }
    return true;
}

bool Monitor::checkValues(const int& value_id, const uint32& value)
{
    return checkValues(value_id, static_cast<int>(value));
}

bool Monitor::checkValues(const int& value_id, const float& value)
{
    return checkValues(value_id, static_cast<double>(value));
}

bool Monitor::checkValues(const int& value_id, const double& value)
{
    Util::Vector<ValueTrigger*>::Iterator iter = _val_triggers.begin();
    for (; iter!=_val_triggers.end(); ++iter)
    {
        if (iter->valueID() == value_id)
        {
            if (iter->check(value))
            {
                triggered(*iter.value());
                if (iter->actions() & Trigger::STOP_MONITOR)
                    return false;
                if (iter->actions() & Trigger::REMOVE_TRIGGER)
                {
                    iter = _val_triggers.erase(iter);
                    --iter;
                }
            }
        }
    }
    return true;
}

RPC::RpcStructValue* Monitor::asRpcValue() const
{
    RPC::RpcStructValue *value = new RPC::RpcStructValue();
    value->set("monitor", _name);
    value->set("monitor_type", _type);
    return value;
}



bool ValueTrigger::check(const int& data) const
{
    if ((_checks & Trigger::GREATER_THAN) && (_checks & Trigger::EQUAL_TO))
        return (data >= _int_value);
    
    if ((_checks & Trigger::LESS_THAN) && (_checks & Trigger::EQUAL_TO))
        return (data <= _int_value);

    if (_checks & Trigger::EQUAL_TO)
        return (data == _int_value);    

    if (_checks & Trigger::GREATER_THAN)
        return (data > _int_value);

    if (_checks & Trigger::LESS_THAN)
        return (data < _int_value);
    
    return false;
}

bool ValueTrigger::check(const uint32& data) const
{
    int val = static_cast<int>(data);
    return check(val);
}

bool ValueTrigger::check(const float& data) const
{
    double val = static_cast<double>(data);
    return check(val);
}

bool ValueTrigger::check(const double& data) const
{
    if ((_checks & Trigger::GREATER_THAN) && (_checks & Trigger::EQUAL_TO))
        return (data >= _float_value);
    
    if ((_checks & Trigger::LESS_THAN) && (_checks & Trigger::EQUAL_TO))
        return (data <= _float_value);

    if (_checks & Trigger::GREATER_THAN)
        return (data > _float_value);

    if (_checks & Trigger::LESS_THAN)
        return (data < _float_value);
    
    return false;    
}

bool KeywordTrigger::check(const String& data) const
{
    if (data.find(_keyword)>=0)
    {
        _match = data;
        return true;
    }
    return false;
}
