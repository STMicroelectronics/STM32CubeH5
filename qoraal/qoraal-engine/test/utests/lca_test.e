decl_name       "least common ancestor test"
decl_version    1

decl_variables {
}

decl_events {
    _evt_Self
    _evt_Transition
    _evt_WriteMenu
}

statemachine lca_test {

    startstate s1111

    state s1 {


    }
    super s1 {


        state s11 {
            event (_evt_Self, s11)
            event (_evt_Transition, s1112)

        }
        super s11 {

            state s111 {
                event (_evt_Self, s111)
                event (_evt_Transition, s1111)
            }
            super s111 {

                state s1111 {
                    event (_evt_Self, s1111)
                    event (_evt_Transition, s2)

                }

                state s1112 {
                    event (_evt_Self, s1112)
                    event (_evt_Transition, s111)

                }

            }

        }

        state s12 {
            event (_evt_Self, s12)
            event (_evt_Transition, s11)


        }

    }

    state s2 {
        event (_evt_Self, s2)
        event (_evt_Transition, s12)

    }

}


statemachine test_controller {

    startstate start

    state start {
        enter       (console_events_register, TRUE)
        enter       (debug_log_statemachine, "lca_test")
        enter       (debug_log_level, LOG_ALL)
        event       (_state_start, menu_ctrl)
    }
    

    state menu_ctrl {
        action          (_state_start, state_event_local, _evt_WriteMenu)

        action          (_evt_WriteMenu, console_writeln, "Control menu:")
        action          (_evt_WriteMenu, console_writeln, "    \\[s] Self.")
        action          (_evt_WriteMenu, console_writeln, "    \\[t] Transition.")
        action          (_evt_WriteMenu, console_writeln, "    \\[?] Help.")
        action          (_evt_WriteMenu, console_writeln, "    \\[D] Dump state.")

        action_eq_e     (_console_char, 's', state_event, _evt_Self)
        action_eq_e     (_console_char, 't', state_event, _evt_Transition)
        action_eq_e     (_console_char, '?', state_event_local, _evt_WriteMenu)
        action_eq_e     (_console_char, 'D', debug_dump)


    }




}