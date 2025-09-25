

#ifndef __PARTS_EVENTS_H__
#define __PARTS_EVENTS_H__

#include "qoraal-engine/config.h"

#include <stdint.h>
#include "parts.h"

extern int32_t      engine_console_event (uint16_t event, uint32_t ch) ;

#define ENGINE_EVENT_DECL(event)    \
        extern const PART_EVENT_T  __engine_event_##event ;

#define ENGINE_EVENT_OFFSET(event)  \
        ((uintptr_t)&__engine_event_##event)

#define ENGINE_EVENT_ID_GET(event)  \
        (uint16_t)((ENGINE_EVENT_OFFSET(event) - (uintptr_t)&__engine_event_base__) / sizeof(PART_EVENT_T))

ENGINE_EVENT_DECL       (_state_start) ;

ENGINE_EVENT_DECL       (_console_char) ;


#define ENGINE_EVENT_CONSOLE_CHAR(ch)           engine_console_event(ENGINE_EVENT_ID_GET(_console_char), ch)



#endif /* __PARTS_EVENTS_H__ */
