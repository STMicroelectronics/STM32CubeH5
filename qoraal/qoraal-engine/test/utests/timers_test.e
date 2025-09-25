decl_name       "timers test"
decl_version    1

decl_variables {
    Timer = 20
}

decl_events {
    _evt_Start
    _evt_Start2
    _evt_WriteMenu
}

statemachine tasks_test {

    startstate s1

    state s1 {
        enter  (console_writeln, "timer test [Timer].")
        enter  (state_keepalive1, 0)
        enter  (state_keepalive2, 0)
		action (_evt_Start, state_keepalive1, 800) 
		action (_evt_Start, state_keepalive2, 600) 
        event  (_evt_Start, s2)

        //action (_evt_Start2, state_timer1_sec, [Timer])
        event  (_evt_Start2, t2)
 
    }

    state t2 {
        enter (state_timer1_sec, [Timer])
        event (_state_timer1, s1)
 
    }

    state s2 {
        enter (state_timeout_sec, 5)
        enter (state_timer1_sec, 1)
        enter (state_timer2_sec, 2)
        enter (state_timer1_sec, 0)
		
		event (_state_timer1, task_error)
		event (_state_timeout, task_error)
		event (_state_timer2, s3)

    }
    
    state s3 {
        enter (state_timeout_sec, 5)
        enter (state_timer1_sec, 2)
        enter (state_timer2_sec, 1)
        enter (state_timer2_sec, 0)
		
		event (_state_timer2, task_error)
		event (_state_timeout, task_error)
		event (_state_timer1, s4)

    }

    state s4 {
        enter (state_timeout_sec, 5)
        enter (state_timer1_sec, 2)
        enter (state_timer2_sec, 1)
        enter (state_timer1_sec, 0)
		
		event (_state_timer1, task_error)
		event (_state_timeout, task_error)
		event (_state_timer2, task_pass)

    }


    state task_pass {
        enter (console_writeln, "Test pass!")
		action (_state_start, state_event, _evt_WriteMenu)
		event (_state_start, s1)

    }


    state task_error {
        enter (console_writeln, "error: terminating test!")
    }
}


statemachine test_controller {

    startstate start

    state start {
        enter       (console_events_register, TRUE)
        enter       (debug_log_statemachine, "tasks_test")
        enter       (debug_log_level, LOG_ALL)
        event       (_state_start, menu_ctrl)
    }
    

    state menu_ctrl {
        action          (_state_start, state_event_local, _evt_WriteMenu)

        action          (_evt_WriteMenu, console_writeln, "Control menu:")
        action          (_evt_WriteMenu, console_writeln, "    1. Test 1.")
        action          (_evt_WriteMenu, console_writeln, "    2. Test 2.")
        action          (_evt_WriteMenu, console_writeln, "    3. Help.")
        action          (_evt_WriteMenu, console_writeln, "    4. Dump state.")

        action_eq_e     (_console_char, '1', state_event, _evt_Start)
        action_eq_e     (_console_char, '2', state_event, _evt_Start2)
        action_eq_e     (_console_char, '?', state_event_local, _evt_WriteMenu)
        action_eq_e     (_console_char, 'D', debug_dump)


    }




}