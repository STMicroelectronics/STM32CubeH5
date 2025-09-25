decl_name       "Toaster Oven"
decl_version    2

/* Load the registry with some default values. */
decl_startup {
    "regadd timer.short      5 &"
    "regadd timer.default    10 &"
    "regadd timer.long       30 &"
    "regadd toaster.name     'My Toaster'"
}

decl_variables {
    Timer = 10  /* Reload value for timer. */
}

/* Actions can dispatch evets into their own state machines. We
   declare a few events here for use by the state machines. */
decl_events {
    _evt_Door
    _evt_OnOff
}

statemachine Toaster_oven {

    /* On startup, transition to Toaster_off. It is
       assumed the door is closed for the example. */
    startstate Toaster_off

    state Door_closed {

        /* Transtioning from Door_open to any of the Door_closed
           substates will call the entry actions defined here. */
        enter       (toaster_lamp, OFF)

        /* For any of the Door_closed substates the Door event
           will transition to the Door_open state. */
        event       (_evt_Door, Door_opened)
        
        /* When the door is opened the lamp is turned on. */
        exit        (toaster_lamp, ON)

    }
    super Door_closed {

        /* All states in this scope takes Door_closed as 
           super state. Entry and Exit actions during state
           state transitions will happen according to the UML
           specification. Super states can be nested. */

        state Toaster_off {

            /* Reset the timer. */
            enter       (state_timer1_sec, 0)

            /* Set the timer. */
            action      (_evt_OnOff, state_timer1_sec, [Timer])

            /* Transition to Toaster_on. */
            event       (_evt_OnOff, Toaster_on)

        }

        state Toaster_on {

            /* On entry, turn the heater on. */
            enter       (toaster_heater, ON)

            /* When the timer expires, transition to Toaster_off. */
            event       (_state_timer1, Toaster_off)
            /* On the OnOff event, transition to Toaster_off. */
            event       (_evt_OnOff, Toaster_off)

            /* On exit, turn the heater off. */
            exit        (toaster_heater, OFF)

        }

    }


    state Door_opened {

        /* When the door is closed, check if the timer is active
           and load the result into the accumulator. */
        action_ld (_evt_Door, [a], state_timer1_active)

        /* If the accumulator is set, transition to Toaster_on. */
        event_if (_evt_Door, Toaster_on)
        /* If the accumulator is clear, transition to Toaster_off. */
        event_nt (_evt_Door, Toaster_off)

    }

}

/* more variable declarations to be appended to the global variables */
decl_variables {
    LogLevel = LOG_ALL /* Debug log level */
    LogOutput = ENABLE /* Toaster output ENABLED/DISABLED */
}

/* more event declarations to be appended to the global events */
decl_events {
    _evt_WriteSettings
    _evt_WriteMenu
    _evt_Misc
}

statemachine Toaster_controller {

    startstate start

    state start {
        enter       (console_events_register, TRUE)
        enter       (debug_log_statemachine, "Toaster_oven")
        enter       (debug_log_level, [LogLevel])
        enter       (debug_log_output, [LogOutput])
        event       (_state_start, menu_ctrl)
    }
    
    state menu {
        action      (_evt_WriteSettings, console_writeln, "")
        action      (_evt_WriteSettings, console_writeln, "'[toaster.name]' settings:")
        action      (_evt_WriteSettings, console_writeln, "    - Timer: [Timer]s")
        action      (_evt_WriteSettings, a_load, [LogLevel])
        action_eq   (_evt_WriteSettings, LOG_ALL, console_writeln, "    - Log Level: ALL")
        action_eq   (_evt_WriteSettings, LOG_TRANSITIONS, console_writeln, "    - Log Level: TRANSITIONS")
        action_eq   (_evt_WriteSettings, LOG_REPORT, console_writeln, "    - Log Level: REPORT")
        action      (_evt_WriteSettings, a_load, [LogOutput])
        action_eq   (_evt_WriteSettings, DISABLE, console_writeln, "    - Log Output: DISABLE")
        action_ne   (_evt_WriteSettings, DISABLE, console_writeln, "    - Log Output: ENABLE")

        action_eq_e     (_console_char, 'D', debug_dump)

    }
    super menu {
        state menu_ctrl {
            action          (_state_start, state_event_local, _evt_WriteSettings)
            action          (_state_start, state_event_local, _evt_WriteMenu)

            action          (_evt_WriteMenu, console_writeln, "Control menu:")
            action          (_evt_WriteMenu, console_writeln, "    \\[t] Toaster On/Off.")
            action          (_evt_WriteMenu, console_writeln, "    \\[d] Door Open/Close.")
            action          (_evt_WriteMenu, console_writeln, "    \\[s] Settings.")
            action          (_evt_WriteMenu, console_writeln, "    \\[h] Help.")
            action          (_evt_WriteMenu, console_writeln, "    \\[q] Quit.")

            action          (_console_char, a_load, 0)
            action_eq_e     (_console_char, 't', state_event, _evt_OnOff)
            action_eq_e     (_console_char, 'd', state_event, _evt_Door)
            action_eq_e     (_console_char, 's', a_load, TRUE)
            action_eq_e     (_console_char, 'h', state_event_local, _evt_WriteMenu)
            action_eq_e     (_console_char, '?', state_event_local, _evt_WriteMenu)

            event_if        (_console_char, menu_settings)

        }

        state menu_settings {

            action          (_state_start, state_event_local, _evt_WriteSettings)
            action          (_state_start, state_event_local, _evt_WriteMenu)

            action          (_evt_WriteMenu, console_writeln, "Settings menu:")
            action          (_evt_WriteMenu, console_writeln, "    \\[1] Set [timer.short] second timer.")
            action          (_evt_WriteMenu, console_writeln, "    \\[2] Set [timer.default] second timer.")
            action          (_evt_WriteMenu, console_writeln, "    \\[3] Set [timer.long] second timer.")
            action          (_evt_WriteMenu, console_writeln, "    \\[4] Log REPORT.")
            action          (_evt_WriteMenu, console_writeln, "    \\[5] Log TRANSITIONS.")
            action          (_evt_WriteMenu, console_writeln, "    \\[6] Log ALL.")
            action          (_evt_WriteMenu, console_writeln, "    \\[7] Output ENABLE/DISABLE.")
            action          (_evt_WriteMenu, console_writeln, "    \\[8] Start tress test.")
            action          (_evt_WriteMenu, console_writeln, "    \\[x] Exit setting menu.")
            action          (_evt_WriteMenu, console_writeln, "    \\[q] Quit.")

            action_ld       (_console_char, [a], get, [Timer])
            action_eq_e     (_console_char, '1', a_load, [timer.short])
            action_eq_e     (_console_char, '2', a_load, [timer.default])
            action_eq_e     (_console_char, '3', a_load, [timer.long])
            action_ld       (_console_char, [Timer], get, [a])

            action_ld       (_console_char, [a], get, [LogLevel])
            action_eq_e     (_console_char, '4', a_load, LOG_REPORT)
            action_eq_e     (_console_char, '5', a_load, LOG_TRANSITIONS)
            action_eq_e     (_console_char, '6', a_load, LOG_ALL)
            action_ld       (_console_char, [LogLevel], get, [a])
            action          (_console_char, debug_log_level, [LogLevel])

            action_ld       (_console_char, [a], get, [LogOutput])
            action_eq_e     (_console_char, '7', a_not, DISABLE)
            action_ld       (_console_char, [LogOutput], get, [a])
            action          (_console_char, debug_log_output, [LogOutput])


            action_eq_e     (_console_char, '8', state_event_local, _evt_Misc)

            action          (_console_char, a_load, 0)
            action_eq_e     (_console_char, 'x', a_load, 1)

            event_if        (_console_char, menu_ctrl)
            event_nt        (_console_char, menu_settings)
        
            event           (_evt_Misc, stress_test)

        }

    }
    
    state stress_test {
        
            enter (console_writeln, ">> test")
            
            enter   (state_timer1, 1000) 

            action      (_state_timer1, rand+, 10)
            action_gt   (_state_timer1, 5, console_writeln, ">> test Door")
            action_gt   (_state_timer1, 5, state_event, _evt_Door)
            action_lt   (_state_timer1, 5, console_writeln, ">> test OnOff")
            action_lt   (_state_timer1, 5, state_event, _evt_OnOff)
            action      (_state_timer1, rand, 4)
            action      (_state_timer1, a_inc, 5)
            action      (_state_timer1, state_timer1_sec, [a])

            action_eq_e     (_console_char!, 'D', debug_dump)

            event (_console_char, PREVIOUS)

            exit (console_writeln, "<< test")


    }

}



