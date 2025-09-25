decl_name       "simple html test"
decl_version    1

statemachine html_test {

    startstate html_head

    state html {
        enter (console_writeln, ""
                    "<!DOCTYPE html>\r\n"
                    "<html lang=\"en\">\r\n")
        exit (console_writeln, ""
                    "</html>\r\n")
    }

    super html {
        state html_head {
            enter (console_writeln, "<head>"
                                    "<meta charset=\"UTF-8\">\r\n"
                                    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                                    "<title>Simple Page</title>\r\n"
                                    "<style>\r\n"
                                    "    body {\r\n"
                                    "        font-family: Arial, sans-serif;\r\n"
                                    "        background-color: #FAF7F5;\r\n"
                                    "        color: #333;\r\n"
                                    "        display: flex;\r\n"
                                    "        flex-direction: column;\r\n"
                                    "        justify-content: center;\r\n"
                                    "        align-items: center;\r\n"
                                    "        height: 100vh;\r\n"
                                    "        margin: 0;\r\n"
                                    "    }\r\n"
                                    "    h1 {\r\n"
                                    "        color: #AD231F;\r\n"
                                    "    }\r\n"
                                    "</style>\r\n")
            exit (console_writeln, "</head>\r\n")
            event (_state_start, html_body)
        }
    }

    super html {
        state html_body {
            enter (console_writeln, "<body>"
                                    "<h1>Welcome to My Page</h1>\r\n"
                                    "<p>This is a simple message to brighten your day!</p>\r\n")
            exit (console_writeln, "</body>")
            event (_state_start, done)

        }
    }


    state done {
        // enter (html_done)
        enter (console_writeln, "enter done")
        exit (console_writeln, "exit done")

    }
}
