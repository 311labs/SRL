#ifndef _SRL_RPCVALUE_H_
#define _SRL_RPCVALUE_H_

#include "srl/String.h"
#include "srl/DateTime.h"
#include "srl/io/ByteBuffer.h"
#include "srl/util/Dictionary.h"
#include "srl/util/Vector.h"
#include "srl/xml/XML.h"
#include "srl/app/SmartPointers.h"

namespace SRL
{

    namespace RPC
    {
        /**
        * Represents an Abstracted Datatype to be used with RPC Protocols
        * This base class represents the basic types and should be extended to 
        * represent more complex types
        */
        class SRL_API RpcValue
        {
        public:
 
            /**
            * Creates an RPC Value from the xml parser.  The xml parsers current tag
            * should be a value tag.
            * @param parser XML parser 
            */
            static RpcValue* FromXml(XML::Parser& parser);
            /** return the current rpc object as xml */
            virtual const String& asXml();

            
            /** define the type of RPC Value */
            enum Type {
              TypeInvalid,
              TypeBoolean,
              TypeInt,
              TypeDouble,
              TypeString,
              TypeDateTime,
              TypeBase64,
              TypeNull,
              TypeArray,
              TypeStruct
            };            
            
            /** default contructor */
            RpcValue() : _type(TypeInvalid) { _value.asDateTime = NULL; }         
            /** Construct an RpcValue with a bool value */
            RpcValue(bool value) : _type(TypeInvalid){ set(value); }
            /** Construct an RpcValue with an int value */
            RpcValue(int value) : _type(TypeInvalid){ set(value); }
            /** Construct an RpcValue with a double value */
            RpcValue(double value) : _type(TypeInvalid){ set(value); }            
            /** Construct an RpcValue with a string */
            RpcValue(const String& value) : _type(TypeInvalid){ set(value); }
            /** Construct an RpcValue with a string */
            RpcValue(const char* value) : _type(TypeInvalid){ set(value); }
            /** Construct an RpcValue with a string */
            RpcValue(const DateTime& value) : _type(TypeInvalid){ set(value); }           
            
            /** virtual destructor */
            virtual ~RpcValue() { clear(); }
            /** clear the rpcvalue to the invalid state */
            virtual void clear();
            
            /** returns the current RPC VALUE TYPE */
            Type const &getType() const { return _type; }
            /** return the value of the current rpc object as a string */
            virtual const String& asString();

            /** set the current value of the rpc value */
            void set(const bool& value);
            /** set the current value of the rpc value */
            void set(const int& value);
            /** set the current value of the rpc value */
            void set(const double& value);
            /** set the current value of the rpc value */
            void set(const String& value);
            /** set the current value of the rpc value */
            void set(const char* value);
            /** set the current value of the rpc value */
            void set(const DateTime& value);

            /** return the value of the current rpc object as a boolean */
            const bool& asBool() const { return _value.asBool; }
            /** return the value of the current rpc object as a boolean */
            const int& asInt() const { return _value.asInt; }
            /** return the value of the current rpc object as a boolean */
            const double& asDouble() const { return _value.asDouble; }
            /** return the value of the current rpc object as a boolean */
            const DateTime& asDateTime() const { return *_value.asDateTime; }

        protected:
            /** type of rpc value */
            Type _type;
            /** Value as a String */
            String _str;
            /** Value in XML form */
            String _xml;
            /** value */
            union 
            {
                bool          asBool;
                int           asInt;
                double        asDouble;
                DateTime*     asDateTime;
            } _value;
        };
        
        /**
        * Represents and XML-RPC Struct Value
        *
        */
        class SRL_API RpcStructValue : public RpcValue
        {
        public:        
        
            /** create the struct from the passed in xml parser */
            RpcStructValue(XML::Parser& parser);            
            /** return the current rpc object as xml */
            virtual const String& asXml();            
            
            RpcStructValue(){ _type = RpcValue::TypeStruct; }
            virtual ~RpcStructValue ();
 
            /** clear the rpcvalue to the invalid state */
            virtual void clear();
            
            /** number of members in the struct */
            const uint32& size() const { return _struct.size(); }
            
            /** checks if the structure has a member with the given name */
            bool hasMember(const String& name) const { return _struct.hasKey(name); }
            
            /** returns the rpcvalue for the given name or null if it does not exist */
            RpcValue* get(const String& name);
            
            /** add/set a member to the struct, the struct will be responsible for the rcpvalue  */
            void set(const String& name, RpcValue* value) { _struct.add(name, value); }
            /** set the current value of the rpc value */
            void set(const String& name, const bool& value);
            /** set the current value of the rpc value */
            void set(const String& name, const int& value);
            void set(const String& name, const uint32& value);
            /** set the current value of the rpc value */
            void set(const String& name, const double& value);
            /** set the current value of the rpc value */
            void set(const String& name, const String& value);
            /** set the current value of the rpc value */
            void set(const String& name, const char* value);            
            /** set the current value of the rpc value */
            void set(const String& name, const DateTime& value);            
            
        protected:
            /** Dictionary to hold the struct keys and values */
            Util::Dictionary<String, RpcValue*> _struct;

        };        
        
        /**
        * Represents and XML-RPC Array Value
        *
        */
        class SRL_API RpcArrayValue : public RpcValue
        {
        public:

            /** create the struct from the passed in xml parser */
            RpcArrayValue(XML::Parser& parser);
            
            RpcArrayValue() : RpcValue(), _array(32) { _type = RpcValue::TypeArray; };
            virtual ~RpcArrayValue ();
            
            /** clear the rpcvalue to the invalid state */
            virtual void clear();
            
            /** size of the current array */
            const uint32& size() const { return _array.size(); }
            
            /** return the current rpc object as xml */
            virtual const String& asXml();
            
            /** add rpc value */
            void add(const bool& value) { add(new RpcValue(value)); }
            /** add rpc value */
            void add(const int& value) { add(new RpcValue(value)); }
            /** add rpc value */
            void add(const double& value) { add(new RpcValue(value)); }
            /** add rpc value */
            void add(const String& value) { add(new RpcValue(value)); }
            /** add rpc value */
            void add(const DateTime& value) { add(new RpcValue(value)); }
            /** add rpc value */
            void add(RpcValue* value) { _array.add(value); }
            
            /** returns the rpcvalue for the given name or null if it does not exist */
            RpcValue* get(const uint32& index) const { return _array[index]; }       

        protected:
            /** Dictionary to hold the struct keys and values */
            Util::Vector<RpcValue*> _array;

            
        };

        /**
        * Represents and XML-RPC Base64/Binary Value
        *
        * TODO: this needs to be fixed up to support BinaryReader
        */
        class SRL_API RpcBinaryValue : public RpcValue
        {
        public:
            /** create the struct from the passed in xml parser */
            RpcBinaryValue(XML::Parser& parser);
            
            RpcBinaryValue(){ _type = RpcValue::TypeBase64; }
            virtual ~RpcBinaryValue (){};
            
            /** size of the current byte buffer */
            const uint32& size() const { return _bytes.size(); }
            
            /** return the current rpc object as xml */
            virtual const String& asXml();
            
            /** access the byte buffer */
            IO::ByteBuffer& bytes() { return _bytes; }
            
        protected:
            IO::ByteBuffer _bytes;

        };        
        
        
    } /* RPC */
    
} /* SRL */


#endif /* _SRL_RPCVALUE_H_ */
