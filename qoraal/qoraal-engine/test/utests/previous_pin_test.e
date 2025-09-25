decl_name       "previsous pin test"
decl_version    1

decl_variables {
}

decl_events {
    _evt_Transition
    _evt_Previous
    _evt_WriteMenu
}

statemachine previsous_pin_test {

    startstate s1

    state s1 {
        enter (nop)
        event (_evt_Transition^, s21)

    }

    state s2 {
        enter (nop)
        event (_evt_Previous, PREVIOUS)
    }
    super s2 {
    
        state s21 {
            enter (nop)
            event (_evt_Transition, s22)

        }
        state s22 {
            enter (nop)
            event (_evt_Transition, s23)

        }
        state s23 {
            enter (nop)
            event (_evt_Transition, s21)
            event (_evt_Previous, IGNORE)

        }

    }

}


statemachine test_controller {

    startstate start

    state start {
        enter       (console_events_register, TRUE)
        enter       (debug_log_statemachine, "previsous_pin_test")
        enter       (debug_log_level, LOG_ALL)
        event       (_state_start, menu_ctrl)
    }
    

    state menu_ctrl {
        action          (_state_start, state_event_local, _evt_WriteMenu)

        action          (_evt_WriteMenu, console_writeln, "Test Control menu:")
        action          (_evt_WriteMenu, console_writeln, "    \\[t] Transition.")
        action          (_evt_WriteMenu, console_writeln, "    \\[b] Back.")
        action          (_evt_WriteMenu, console_writeln, "    \\[?] Help.")
        action          (_evt_WriteMenu, console_writeln, "    \\[D] Dump state.")

        action_eq_e     (_console_char, 't', state_event, _evt_Transition)
        action_eq_e     (_console_char, 'b', state_event, _evt_Previous)
        action_eq_e     (_console_char, '?', state_event_local, _evt_WriteMenu)
        action_eq_e     (_console_char, 'D', debug_dump)


    }




}