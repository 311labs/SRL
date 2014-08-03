#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "srl/String.h"
#include "srl/sys/Thread.h"
#include "srl/util/Vector.h"
#include "srl/Singleton.h"

namespace SRL
{
    namespace System
    {
        /** Information and control of keyboard state/events */
        class SRL_API Keyboard : public Thread, public Singleton<Keyboard>
        {
            friend class Singleton<Keyboard>;
        public:
            /** KEY STATES */
            enum KEY_STATE {
              KEY_UP=0,
              KEY_DOWN=1,
              KEY_ON=2,
              KEY_OFF=3
            };
            /** KEY MODIFIERS */
            enum MODIFIERS {
              MODKEY_NONE = 0x0000,
              MODKEY_SHIFT = 0x0001,
              MODKEY_CTL = 0x0002,
              MODKEY_WIN = 0x0004,
              MODKEY_ALT = 0x0008
            };

            struct HotKey
            {
                HotKey(){}
                HotKey(int _id, int _mod, int _kc, bool _pt, int _ac) : 
                id(_id), modifiers(_mod), keycode(_kc), passthrough(_pt), action(_ac), active(false){}
                /** check if date is estring2ual to the passed in date */
                friend SRL_API bool operator==(const HotKey &hk1, const HotKey &hk2)
                {
                    return (hk1.id == hk2.id);
                }
                int id;
                int modifiers;
                int keycode;
                bool passthrough;
                int action;
                bool active;
            };
            
            /** Keyboard Listener */
            class Listener
            {
            public:
                virtual ~Listener(){}
                virtual void hotkey_event_down(const uint32& id)=0;
                virtual void hotkey_event_up(const uint32& id)=0;
            };
            
            /** Keyboard System */
            Keyboard(bool start=true);
            virtual ~Keyboard();
            /** get the state of the specified key */
            KEY_STATE getKeyState(int keycode);
            
            /** set the listener
            * TODO Support multiple listeners */
            void addListener(Listener* listener){ _listener = listener; }
            /** remove a listener
            * TODO Support multiple listeners */
            void removeListener(Listener* listener){ _listener = NULL; }
            /**
             * Registers a hotkey. 
             * @param modifier      ALT, SHIFT, CTRL, WIN or combination of
             * @param keycode       Ascii keycode, 65 for A, 66 for B etc
             * @param passthrough   flag for pass through state of hotkey
             * @returns             the id for the hotkey created
            */
            uint32 registerHotkey(int modifier, int keycode, bool passthrough=true);
            /**
            * Unregisters a hotkey
            * @param id     identifier for the hotkey to be removed
            */
            void unregisterHotkey(const uint32& id);

        protected:
            virtual bool initial();
            virtual bool run();
            virtual void final();

            virtual void _hotkey_pressed(HotKey *hk);
            virtual void _hotkey_released(HotKey *hk);

            /* vector of registered hotkeys */
            Util::Vector<HotKey> _hotkeys;
            /** hotkey ids */
            uint32 _ids;
            /** lister to send events to */
            Listener* _listener;
        };
        
    }
}


#endif /* _KEYBOARD_H_ */

