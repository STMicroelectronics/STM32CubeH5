decl_name       "deferred event test"
decl_version    1

decl_variables {
}

decl_events {
    _evt_700
    _evt_701
    _evt_Transition
    _evt_WriteMenu
}

statemachine deferred_test {

    startstate s1

    state s1 {
        enter (nop)
        event (_evt_Transition, s2)
        event (_evt_700, s_error)
        event (_evt_701, s_error)

        deferred (_evt_700) 
        deferred (_evt_701) 

    }

    state s2 {
        enter (nop)
        event (_evt_Transition, s3)
        event (_evt_701, s_error)

        deferred (_evt_701) 

    }
    
    state s3 {
        enter (nop)
        event (_evt_Transition, s1)

    }

    state s_error {
        enter (console_writeln, "error: terminating test!")
    }
}


statemachine test_controller {

    startstate start

    state start {
        enter       (console_events_register, TRUE)
        enter       (debug_log_statemachine, "deferred_test")
        enter       (debug_log_level, LOG_ALL)
        event       (_state_start, menu_ctrl)
    }
    

    state menu_ctrl {
        action          (_state_start, state_event_local, _evt_WriteMenu)

        action          (_evt_WriteMenu, console_writeln, "Control menu:")
        action          (_evt_WriteMenu, console_writeln, "    \\[t] Transition.")
        action          (_evt_WriteMenu, console_writeln, "    \\[1] Event 0x700.")
        action          (_evt_WriteMenu, console_writeln, "    \\[2] Event 0x701.")
        action          (_evt_WriteMenu, console_writeln, "    \\[?] Help.")
        action          (_evt_WriteMenu, console_writeln, "    \\[D] Dump state.")

        action_eq_e     (_console_char, 't', state_event, _evt_Transition)
        action_eq_e     (_console_char, '1', state_event, _evt_700)
        action_eq_e     (_console_char, '2', state_event, _evt_701)
        action_eq_e     (_console_char, '?', state_event_local, _evt_WriteMenu)
        action_eq_e     (_console_char, 'D', debug_dump)


    }




}