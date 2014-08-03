
#ifndef __SRL_SharedObject__
#define __SRL_SharedObject__

#include "srl/Serializer.h"
#include "srl/Mutex.h"

namespace SRL
{
	/**
	* @author Ian Starnes
	* $Author: ians $
	* $Revision: 184 $
	* $Date: 2005-10-17 18:26:59 -0400 (Mon, 17 Oct 2005) $
	* @short Shared Object that deletes itself when not used anymore
	*
	* A Shared object that is NOT THREAD SAFE.  When a user holds onto a reference of this
	* object a call to checkout should be made.. when done a call to release.
	*/
	class SRL_API SharedObject : public Serializable
	{
	public:
		/** Constructor */
		SharedObject();
		/** Destructor */
		virtual ~SharedObject();
		/** Checks out the object */
		void checkout() const throw();
		/** Releases the object */
		void release() const throw();
		/** Reference Count */
		const integer referenceCount() const throw();
		/** object id (unique for each object)*/
		const uint32 id() const throw();
		/** generate an ID */
		void _genID() const throw();
		/** serialize the current object to the IOObject */
		virtual void serialize(Serializer *serializer) const{};
		/** unserialize the object from the IOObject */
		virtual void deserialize(Serializer *serializer){};
	protected:
		/** current id */
		mutable uint32 _id;
	private:
		/** Reference counter */
		mutable integer _referenceCount;
		// STATIC
		/** last id generated */
		static uint32 __lastID;
	};

	namespace TS
	{
		/**
		* @author Ian Starnes
		* $Author: ians $
		* $Revision: 184 $
		* $Date: 2005-10-17 18:26:59 -0400 (Mon, 17 Oct 2005) $
		* @short A Shared Object that is thread safe!
		*/
		class SRL_API SharedObject
		{
		public:
			/** Constructor */
			SharedObject();
			/** Destructor */
			virtual ~SharedObject();
			/** Checks out the object */
			void checkout() const throw();
			/** Releases the object */
			void release() const throw();
			/** Reference Count */
			const integer referenceCount() const throw();
			/** object id (unique for each object)*/
			const uint32 id() const throw();
			/** generate an ID */
			void _genID() const throw();
			/** serialize the current object to the IOObject */
			virtual void serialize(Serializer *serializer) const{};
			/** unserialize the object from the IOObject */
			virtual void deserialize(Serializer *serializer){};
		protected:
			/** current id */
			mutable uint32 _id;
		private:
			/** Critical section the thread safety */
			mutable Sys::Mutex _critSec;
			/** Reference counter */
			mutable integer _referenceCount;
			// STATIC
			/** last id generated */
			static uint32 __lastID;
			/** static mutex */
			static Sys::Mutex __mutex;
		
		};


	}
}
#endif // __SRL_SharedObject__

