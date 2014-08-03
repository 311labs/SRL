#ifndef __XMLRPCUTIL__
#define __XMLRPCUTIL__

#include "srl/String.h"
#include "srl/fs/TextFile.h"
#include "srl/Exceptions.h"

namespace SRL
{
    namespace XML
    {
        /** Thrown when parsing invalid XML  */
        class SRL_API InvalidXML : public SRL::Errors::IOException
        {
        public:
        	/** default constructor */
        	InvalidXML(String message): SRL::Errors::IOException(message){};
        };
        
        /**
        * Simple XML Parser (Pull Parser)
        * A pull parser creates an iterator that sequentially visits the various elements, attributes,
        * and data in an XML document. Code which uses this 'iterator' can test the current item
        * (to tell, for example, whether it is a start or end element, or text), 
        * and inspect its attributes (local name, namespace, values of XML attributes, value of text, etc.),
        * and can also move the iterator to the 'next' item. The code can thus extract information from the
        * document as it traverses it. Pull-parsing code can be more straightforward to understand and
        * maintain than SAX parsing code.
        */
        class SRL_API Parser
        {
        public:
            // forward decleration
            class Element;
            /**
            * Attribute Iterator
            * Allows you to iterate through an elements attributes
            */
            class SRL_API Attribute
            {
            friend class Element;
            public:
                /** copy constructor */
                Attribute(const Attribute &attr);
                /** assignment operator */
                Attribute& operator=(const Attribute& attr);
                /** returns the name of the attribute */
                const String& name() const{ return _name; }
                /** returns the value of the attribute as a string */
                const String& value() const{ return _value; }
                /** returns the value of the attribute as an int */
                int intValue() const{ return String::Int(_value); }
                /** returns the value of the attribute as a long */
                long longValue() const{ return String::Long(_value); }
                /** returns the value of the attribute as a float */
                float floatValue() const{ return String::Float(_value); }
                /** returns the value of the attribute as a double */
                double doubleValue() const{ return String::Double(_value); }
                
            protected:
                Attribute(Element& element) : _element(element), _name(64), _value(64){}
                Element &_element;
                String _name, _value;
            };
            
            /** 
            * Element Iterator
            * Allows you to iterate through the XML data (forwards and backwords)
            */
            class SRL_API Element
            {
            friend class Parser;
            public:
                /** returns the name of the element */
                const String& name() const{ return _name; }
                /** returns true if the element is a closing element */
                bool isClosing() const { return _is_closing; }
                /** returns true if the element has content */
                bool hasContent() throw(InvalidXML){ return _parser.hasContent(); }
                /** returns the content for the current element as a string */
                const String& getContent() throw(InvalidXML);
                /** returns the content as an integer */
                void getContent(int& data) throw(InvalidXML);
                /** returns the content as an long */
                void getContent(long& data) throw(InvalidXML);
                /** returns the content as an float */
                void getContent(float& data) throw(InvalidXML);
                /** returns the content as an double */
                void getContent(double& data) throw(InvalidXML);
                /** returns the content as an boolean */
                void getContent(bool& data) throw(InvalidXML);
                /** returns the content as an DateTime */
                //void getContent(bool& data) throw(InvalidXML);             
                
                /** returns true if the element has attributes */
                bool hasAttributes() const{ return _has_attributes; }
                /* returns an attribute iterator */
                Attribute* getNextAttribute();
                /* find attribute by name */
                Attribute* findAttribute(const String& name);
                
            protected:
                Element(Parser& parser, bool null_element=false);
                ~Element();
                void clear();
                
            protected:
                Parser &_parser;
                Attribute _attribute;
                String _name, _attributes, _content;
                bool _is_closing, _has_attributes, _is_null;
                uint32 _apos;
            };
            
            /** Constructor that takes an XML File */
            Parser(FS::TextFile* xml_file);
            /** Constructor that takes raw xml data */
            Parser(const String& xml);
            virtual ~Parser();
            
            /**
            * Parses the next element in the XML data
            * returns either a pointer(the pointer is reused so do not delete) to the element or NULL
            */
            Element* getNextElement() throw(InvalidXML);
            /**
            * returns a pointer to the current element or NULL if there is none
            */
            Element* getCurrentElement() { return &_element; }
            /**
            * returns a pointer to the previous element
            */
            Element* getPreviousElement() throw(InvalidXML);
            
            
            /**
            * Find the next Element with the given name
            */
            Element* findNextElement(const String& name, bool is_closing=false) throw(InvalidXML);

            /**
            * Find the previous Element with the given name
            */
            Element* findPreviousElement(const String& name, bool is_closing=false) throw(InvalidXML);

            
                
        protected:
            /** checks to see if the current element has content data */
            bool hasContent() throw(InvalidXML);
            /** returns the text for the current tag */
            void parseContent();

        protected:
            Element _element, _null_element;
            FS::TextFile* _xml_file;
            String _xml;
            uint32 _pos;
            /** reads more xml from the xml file filling the xml buffer */
            bool _getXML(bool reverse=false);
            void _parseElement();
        };
        /**
        * extracts all of the inner content of the specified tag
        *
        * @param tag    tag to grab the inner text from
        * @param xml    String that represents the xml
        * @param offset pointer to the offset in the xml where the tag starts
        * @returns String that contains all of the data inside the tag given
        */
        extern SRL_API String parseTag(const char* tag, const String& xml, int* offset);

        /**
        * scans the xml starting from the offset for the specified tag
        * and returns true if the tag was found, also updates the offset to 1 char
        * after the tag
        * 
        * @param tag    tag to scan for
        * @param xml    String that represents the xml
        * @param offset pointer to the offset in the xml where the tag starts
        * @returns True if a tag is present at the offset, false otherwise.
        */
        SRL_API bool findTag(const char* tag, const String& xml, int* offset);
        
        /**
        * Returns the next tag and updates offset to the char after the tag,
        * or empty string if the next non-whitespace character is not '<'
        *
        * @param xml    String that represents the xml
        * @param offset pointer to the offset in the xml where to start
        * @returns the next tag name in the xml from the last offset
        */
        SRL_API String getNextTag(const String& xml, int* offset);

        /**
        * Returns true if the tag is found at the specified offset 
        * (modulo any whitespace) and updates offset to the char after the tag.
        * 
        * @param tag    tag to scan for
        * @param xml    String that represents the xml
        * @param offset pointer to the offset in the xml where the tag starts
        * @returns True if a tag is present at the offset, false otherwise.
        */
        SRL_API bool nextTagIs(const char* tag, const String& xml, int* offset);

        /**
        * Returns a copy of the passed in string as XML encoded
        * 
        * @param raw    String that needs to be encoded
        * @returns String that is a XML encoded copy of the passed in string
        */
        SRL_API String encode(const String& raw);
        
        /**
        * Decodes an XML encoded string
        * 
        * @param raw    String that needs to be decoded
        * @returns String that is a XML decoded copy of the passed in string
        */
        SRL_API String decode(const String& encoded);
        
        
    } /* XML */
    
} /* SRL */


#endif