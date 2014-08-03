/********************************************************************************
* Copyright (C) 1999,2004 by Ian C Starnes.   All Rights Reserved.        *
*********************************************************************************
* Sub Authors: J. Shane Frasier, Shaun Grant, Sastring2ib Ali, Mark Feldhousen
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
*******************************************************************************                                                     
* Last $Author: ians $     $Revision: 184 $                                
* Last Modified $Date: 2005-10-17 18:26:59 -0400 (Mon, 17 Oct 2005) $                                  
******************************************************************************/

#ifndef __SRL_INI_PARSER__
#define __SRL_INI_PARSER__

#include "srl/String.h"
#include "srl/Exceptions.h"
#include "srl/util/Dictionary.h"

namespace SRL
{
    namespace App
    {
	
    	/** Simple INI Config Parser */
    	class SRL_API IniParser
    	{
            public:
    		/** map of properties */
    		typedef Util::Dictionary<String, String> PropertyMap;
    		/** Ini Config Section */
    		class SRL_API Section
    		{
    		public:
    			Section(const SRL::String &name);
    			~Section();
    			/** get name of current section */
    			const String& getName(){ return _name; }
			
    			/** returns an iterator to the first section */
    			PropertyMap::Iterator begin() { return _properties.begin(); }
    			/** end of sections */
    			PropertyMap::Iterator end() { return _properties.end(); }
			
    			/** return the field as a string */
    			const String& getString(const String &name, const String &default_value);
    			/** return the field as an integer */
    			int getInteger(const String &name, const int &default_value);
    			/** return the field as an floating point 64 bit */
    			float64 getDouble(const String &name, const float64 &default_value);
			
    			/** put the string property in the section */
    			void putString(const String &name, const String &value);
    			/** put the integer property in the section */
    			void putInteger(const String &name, const int &value);
    			/** put the string property in the section */
    			void putDouble(const String &name, const float64 &value);
    		protected:
    			/** name of section */
    			String _name;
    			/** dictionary to store all values */
    			PropertyMap _properties;
    		};
    		/** HashMap of Sections */
    		typedef Util::Dictionary<String, Section*> SectionMap;
    	public:
    		/** */
    		IniParser(const String &filename);
    		~IniParser();
		
    		/** returns an iterator to the first section */
    		SectionMap::Iterator begin() { return _sections.begin(); }
    		/** end of sections */
    		SectionMap::Iterator end() { return _sections.end(); }
		
    		/** returns a section by name */
    		Section *getSection(const String& name);
		
    		/** remove all sections and properties */
    		void clear();
		
    		/** reparses the config file first clearing all existing values */
    		void reparse();
    		/** reparses the config file first clearing all existing values */
    		void parse(const String& filename);    		
    		/** saves the current sections to file */
    		void save();
    		/** saves the current sections to file */
    		void saveTo(const String& filename);    		
    	protected:
    		String _filename;
    		SectionMap _sections;
    	};
    	
        IniParser& GetGlobalConfig();
	}
}

#endif // __SRL_INI_PARSER__

