decl_name       "tic-tac-toe"
decl_version    1

decl_events {
    _tictac_tick
    _tictac_restart
}

statemachine tictactoe {

    startstate ready

    state ready {
        /*
         * Entering the ready state signals readiness (html_ready) to render HTML 
         * when the _html_render event is received.
         */
        enter (html_ready)
        action (_tictac_restart, tictac_restart)
        action (_tictac_tick, tictac_play, [e])
        event (_html_render, html_head)

        /*
         * On receiving _html_render, transition to html_head.
         * The exit action begins the "text/html" response.
         * All subsequent html_emit calls append content to the response.
         */
        exit (html_response, HTML)
    }

    state html {
        enter (html_emit,       "<!DOCTYPE html>\r\n"
                                "<html lang=\"en\">\r\n")
        /*
         * Exiting the html superstate closes the document by emitting the </html> tag.
         */
        exit (html_emit,        "</html>\r\n")
    }

    super html {
        state html_head {
            /*
             * Engine state machine commands are single-line, 
             * but multi-line text blocks are supported. 
             * The closing bracket must be on the same line as the final text line.
             */
            enter (html_emit,   "<head>\r\n"
                                "<meta charset=\"UTF-8\">\r\n"
                                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                                "<title>Tic-Tac-Toe</title>\r\n"
                                "<link rel=\"stylesheet\" href=\"/engine/tictaccss\">\r\n")
            event (_state_start, html_board_title)
            exit (html_emit,    "</head>\r\n")
        }
    }

    super html {
        /*
         * States can be nested. You can either nest them directly or repeat the "super" identifier.
         */
        state html_body {
            enter (html_emit,   "<body>\r\n")
            /*
             * html_subst_emit enables token substitution for identifiers in square brackets ([]).
             * Escape brackets if substitution is not intended.
             */
            exit (html_subst_emit,    "<button class=\"restart-btn\" onclick=\"window.location.href='/engine/tictactoe/[_tictac_restart]'\">Restart</button>\r\n"
                                "<button class=\"restart-btn\" onclick=\"window.location.href='/index'\">Take Me Home</button>"
                                "</body>")
        }

        super html_body {
            state html_board_title {
                action (_state_start, html_emit,                        "<h1>Tic-Tac-Toe</h1>\r\n")
                action_ld (_state_start, [a], tictac_status)
                action_eq (_state_start, TICTAC_DRAW,       html_emit,  "<div id=\"winner-message\" class=\"winner\"> Draw </div>\r\n\r\n")
                action_eq (_state_start, TICTAC_PLAYER_WIN, html_emit,  "<div id=\"winner-message\" class=\"winner\"> 👑 Player Wins! 👑 </div>\r\n")
                action_eq (_state_start, TICTAC_AI_WIN,     html_emit,  "<div id=\"winner-message\" class=\"winner\"> 🎉 AI Wins! 🎉 </div>\r\n")
                event (_state_start, html_board_cell)
            }
           
            state html_board {
                /*
                 * Reset the register [r] (cell counter) to zero before rendering the board.
                 */
                enter (r_load, 0)
                enter (html_emit,   "<div class=\"board\">\r\n")
                exit (html_emit,    "</div>\r\n")
            }

            super html_board {
                state html_board_cell {
                    action_ld (_state_start, [a], tictac_cell, [r])

                    /*
                     * If a cell is open, render it as a clickable link triggering a _tictac_tick event.
                     * The cell number is passed via the [e] event register.
                     */
                    action_eq (_state_start, TICTAC_OPEN,   html_subst_emit,"<div class=\"cell\">"
                                                                            "<a href=\"/engine/tictactoe/[_tictac_tick]/[r]\" "
                                                                            "class=\"invisible-link\"></a></div>\r\n")

                    action_eq (_state_start, TICTAC_PLAYER,     html_emit,  "<div class=\"cell x\"></div>\r\n")
                    action_eq (_state_start, TICTAC_AI,         html_emit,  "<div class=\"cell o\"></div>\r\n")
                    action_eq (_state_start, TICTAC_PLAYER_BLINK,   html_emit,  "<div class=\"cell x blink\"></div>\r\n")
                    action_eq (_state_start, TICTAC_AI_BLINK,       html_emit,  "<div class=\"cell o blink\"></div>\r\n")

                    /*
                     * Increment register [r] after processing each cell.
                     * Once [r] reaches 9, set accumulator [a] and return to "ready".
                     * Otherwise, continue rendering the next cell.
                     */
                    action (_state_start, r_inc, 9)
                    event_nt (_state_start, html_board_cell)
                    event_if (_state_start, ready)
                }
            }     
        }
    }
}

statemachine welcome {

    startstate ready

    state ready {
        enter (html_ready)
        action (_html_render, html_response, HTML)
        event (_html_render, html_head)

    }

    state html {
        enter (html_emit,       "<!DOCTYPE html>\r\n"
                                "<html lang=\"en\">\r\n")
        exit (html_emit,        "</html>\r\n")
    }

    super html {
        state html_head {
            enter (html_emit,   "<head>"
                                "<meta charset=\"UTF-8\">\r\n"
                                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                                "<title>Simple Page</title>\r\n"
                                "<link rel=\"stylesheet\" href=\"/engine/welcomecss\">\r\n")
            exit (html_emit,    "</head>\r\n")
            event (_state_start, html_body)
        }
    }

    super html {
        state html_body {
            enter (html_emit,   "<body>"
                                "<h1>Welcome to My Page</h1>\r\n"
                                "<p>This is a simple message to brighten your day!</p>\r\n")
            exit (html_emit,    "</body>")
            event (_state_start, ready)

        }
    }


}

statemachine tictaccss {

    startstate ready

    state ready {
        enter (html_ready)
        action (_html_render, html_response, CSS)
        event (_html_render, emit_css)

    }

    // Start state to emit CSS
    state emit_css {
        // If your engine supports something like (set_header, "Content-Type: text/css") do it here
        // or configure it in your route definition externally.

        enter (html_emit,   ""
            "body {\r\n"
            "    display: flex;\r\n"
            "    flex-direction: column;\r\n"
            "    align-items: center;\r\n"
            "    justify-content: center;\r\n"
            "    height: 100vh;\r\n"
            "    font-family: \"Comic Sans MS\", cursive, sans-serif;\r\n"
            "    background: radial-gradient(circle, #1e1e2e 0%, #0c0917 90%);\r\n"
            "    color: #e2e2e2;\r\n"
            "    margin: 0;\r\n"
            "}\r\n"
            "\r\n"
            ".board {\r\n"
            "    display: grid;\r\n"
            "    grid-template-columns: repeat(3, 100px);\r\n"
            "    grid-template-rows: repeat(3, 100px);\r\n"
            "    gap: 0;\r\n"
            "    box-shadow: 0 0 25px rgba(255, 0, 255, 0.5);\r\n"
            "    transition: box-shadow 0.5s ease;\r\n"
            "}\r\n"
            ".board:hover {\r\n"
            "    box-shadow: 0 0 45px rgba(255, 0, 255, 0.9);\r\n"
            "}\r\n"
            "\r\n"
            ".cell {\r\n"
            "    width: 100px;\r\n"
            "    height: 100px;\r\n"
            "    display: flex;\r\n"
            "    align-items: center;\r\n"
            "    justify-content: center;\r\n"
            "    font-size: 2em;\r\n"
            "    font-weight: bold;\r\n"
            "    text-align: center;\r\n"
            "    background-color: transparent;\r\n"
            "    box-sizing: border-box;\r\n"
            "    border-bottom: 6px solid rgba(255, 0, 255, 0.8);\r\n"
            "    border-right: 6px solid rgba(255, 0, 255, 0.8);\r\n"
            "    color: #ffffff;\r\n"
            "    transition: transform 0.3s ease;\r\n"
            "    position: relative;\r\n"
            "}\r\n"
            ".cell:hover {\r\n"
            "    transform: scale(1.1);\r\n"
            "}\r\n"
            "\r\n"
            "/* We'll use pseudo-elements for X or O */\r\n"
            ".cell::before,\r\n"
            ".cell::after {\r\n"
            "    content: \"\";\r\n"
            "    position: absolute;\r\n"
            "    top: 50%;\r\n"
            "    left: 50%;\r\n"
            "    transform: translate(-50%, -50%);\r\n"
            "}\r\n"
            "\r\n"
            ".cell.x::before,\r\n"
            ".cell.x::after {\r\n"
            "    width: 70%;\r\n"
            "    height: 6px;\r\n"
            "    background-color: #fff;\r\n"
            "    border-radius: 4px;\r\n"
            "}\r\n"
            ".cell.x::before {\r\n"
            "    transform: translate(-50%, -50%) rotate(45deg);\r\n"
            "}\r\n"
            ".cell.x::after {\r\n"
            "    transform: translate(-50%, -50%) rotate(-45deg);\r\n"
            "}\r\n"
            "\r\n"
            ".cell.o::before {\r\n"
            "    width: 70%;\r\n"
            "    height: 70%;\r\n"
            "    border: 6px solid #fff;\r\n"
            "    border-radius: 50%;\r\n"
            "}\r\n"
            ".cell.o::after {\r\n"
            "    display: none;\r\n"
            "}\r\n"
            "\r\n"
            "/* Remove borders on last column & row */\r\n"
            ".cell:nth-child(3n) {\r\n"
            "    border-right: none;\r\n"
            "}\r\n"
            ".cell:nth-child(7),\r\n"
            ".cell:nth-child(8),\r\n"
            ".cell:nth-child(9) {\r\n"
            "    border-bottom: none;\r\n"
            "}\r\n"
            "\r\n"
            ".restart-btn {\r\n"
            "    font-family: inherit;\r\n"
            "    margin-top: 20px;\r\n"
            "    padding: 10px 20px;\r\n"
            "    font-size: 1.2em;\r\n"
            "    cursor: pointer;\r\n"
            "    background-color: #fff;\r\n"
            "    color: #0c0917;\r\n"
            "    border: none;\r\n"
            "    border-radius: 8px;\r\n"
            "    transition: background-color 0.3s ease;\r\n"
            "}\r\n"
            ".restart-btn:hover {\r\n"
            "    background-color: #ececec;\r\n"
            "}\r\n"
            "\r\n"
            "/* Invisible link that fills the entire cell */\r\n"
            ".invisible-link {\r\n"
            "    display: block;\r\n"
            "    width: 100%;\r\n"
            "    height: 100%;\r\n"
            "    text-decoration: none;\r\n"
            "    color: transparent;\r\n"
            "    position: absolute;\r\n"
            "    top: 0;\r\n"
            "    left: 0;\r\n"
            "    z-index: 1;\r\n"
            "}\r\n"
            "@keyframes blink-symbol {\r\n"
            "    0%, 100% { opacity: 1; }\r\n"
            "    50% { opacity: 0; }\r\n"
            "}\r\n"
            ".cell.blink::before,\r\n"
            ".cell.blink::after {\r\n"
            "    animation: blink-symbol 0.8s step-start 3;\r\n"
            "}\r\n"            
            "@keyframes glow {\r\n"   
            "    0%, 100% { text-shadow: 0 0 10px rgba(255, 255, 0, 0.8), 0 0 20px rgba(255, 165, 0, 0.6); }\r\n"   
            "    50% { text-shadow: 0 0 20px rgba(255, 255, 0, 1), 0 0 40px rgba(255, 165, 0, 0.9); }\r\n"   
            "}\r\n"   
            ".winner {\r\n"   
            "    font-size: 2.5em;\r\n"   
            "    font-weight: bold;\r\n"   
            "    color: #ffcc00;\r\n"   
            "    text-align: center;\r\n"   
            "    margin-bottom: 15px;\r\n"   
            "    animation: glow 1s infinite alternate;\r\n"   
            "}\r\n"   
            ".hidden {\r\n"   
            "    display: none;\r\n"   
            "}\r\n")

            event (_state_start, ready)

    }
}

statemachine welcomecss {

    startstate ready

    state ready {
        enter (html_ready)
        action (_html_render, html_response, CSS)
        event (_html_render, emit_css)

    }

    // Start state to emit CSS
    state emit_css {
            enter (html_emit,   "body {\r\n"
                            "        font-family: Arial, sans-serif;\r\n"
                            "        background: radial-gradient(circle, #1e1e2e 0%, #0c0917 90%);\r\n"
                            "        color: #333;\r\n"
                            "        display: flex;\r\n"
                            "        flex-direction: column;\r\n"
                            "        justify-content: center;\r\n"
                            "        align-items: center;\r\n"
                            "        height: 100vh;\r\n"
                            "        margin: 0;\r\n"
                            "}\r\n"
                            "h1 {\r\n"
                            "        color: #AD231F;\r\n"
                            "}\r\n")
            event (_state_start, ready)
    }

}

