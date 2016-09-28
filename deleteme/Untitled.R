library(irced)
library(jsonlite)

# probably should/will turn this into an S4 or reference class
my_bot <- function(session, event, origin, params, count) {

  switch(
    event,
    LIST = { },
    PART = { bot_cmd_quit(session, "") },
    CHANNEL = { if (origin != "keryx") {
      if (params[2] == ">>QUIT") {
        bot_cmd_part(session, params[1])
      } else {
        res <- sample(c(FALSE, TRUE), 1)
        if (res) {
          bot_cmd_msg(session, params[1], sprintf("Something  %d", sample(100, 1)))
        }
      }
    }},
    CONNECT = { bot_cmd_join(session, "#builds", "") },

    message(
      sprintf(
        "%s\n",
        toJSON(
          list(
            EVENT=unbox(event),
            ORIGIN=unbox(origin),
            PARAMS=params
          ),
          pretty=TRUE)
      )
    )
  )

}

irc_server("irc.rud.is") %>%
  set_channel("#builds") %>%
  bot_connect(my_bot) %>%
  start_bot()
