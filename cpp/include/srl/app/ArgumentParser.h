/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*******************************************************************************/

#ifndef __SRL_ArgParser__
#define __SRL_ArgParser__

#include "srl/String.h"
#include "srl/util/Dictionary.h"

namespace SRL
{
    namespace App
    {
        /** A Command Line Argument that has no value, it exists or doesn't */
        class SRL_API Argument
        {
        public:
            /** Constructor
                @param name     name of argument
                @param keys     a string list of keys 'key1, key2, key3'
                @param help     string to display for help message
            */
            Argument(const String& name, const String& keys, const String& help);
            /** Constructor
                @param name     name of argument
                @param keys     a string list of keys 'key1, key2, key3'
                @param help     string to display for help message
                @param default  default value
            */
            Argument(const String& name, const String& keys, 
                     const String& help, const String& default_value);
            /** destructor */
            ~Argument();

            /** flag if has value */
            const bool& hasValue()const{ return _has_value; }
            /** flag if was parsed */
            const bool& wasParsed()const{ return _was_parsed; }

            /** get the current keys */
            String* getKeys(){ return _keys;}
            /** get the number of keys */
            int getKeyCount(){ return _key_count;}
            /** get the help string */
            const String& getHelp() const{ return _help;}

            /** set parsed flag to true */
            void setParsed(){ _was_parsed = true; }
            /** set the value */
            void setValue(const String& value){ _value = value;}
            /** get the current value */
            const String& getValue() const{ return _value; }                
        
            /** return the value has an int */
            int getIntValue() const{ return String::Int(_value);}
            /** return the value has an int */
            float getFloatValue() const{ return String::Float(_value);}
    		/** return boolean value */
    		bool getBoolValue() const;
                
            /** test if this argument has the key */
            bool hasKey(const String& key);
        
        protected:
            /** parse keys into list */
            void parseKeys(const String& keys);
            /** name of the argument */
            String _name;
            /** list of keys example: '-v', '--verbose' */
            String *_keys;
            /** number of keys */
            int _key_count;
            /** help string */
            String _help;
            /** the current value */
            String _value;
            /** flag if has value */
            bool _has_value;
            /** flag if parsed */
            bool _was_parsed;
        };
    
        // TODO this needs to be cleaned up
        // for one this should be optionparser and argumentparger
    
        /** A Command Line Argument Parser */
        class SRL_API ArgumentParser
        {
        public:
            ArgumentParser(const String& title, const String& description,
            const String& version);
            virtual ~ArgumentParser();
        
            /** add argument */
            void add(const String& name, const String& keys, const String& help);
            /** add value based argument */
            void add(const String& name, const String& keys, 
                     const String& help, const String& default_value);
        
            /** get executable name */
            const String& getExeName() const { return _program;}
        
            /** get argument */
            const Argument* get(const String& name){ return _arguments.get(name);}
        
            /** get array of unparsed arguments */
            String* getArgs(){ return _args;}
            
            /** get array of unparsed arguments */
            const String& getArgs(int index){ return _args[index];}
            
            /** get number of unparsed arguments */
            uint32 getArgCount(){ return _arg_count;}
        
            /** parse the arguments */
            void parse(int argc, const char* argv[]);
            /** print the help */
            void printHelp();
            /** find argument by key */
            Argument* findArgByKey(const String& key);
        
        protected:
            String *_args;
            String _title, _description, _version;
            uint32 _arg_count;
            String _program;
            Util::Dictionary<String, Argument*> _arguments;
        };
    }
}

#endif //__SRL_ArgParser__

