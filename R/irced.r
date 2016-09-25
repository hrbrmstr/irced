#' Create an IRC server object
#'
#' @param host_or_ip IRC server FQDN or IP address
#' @param port IRC server port
#' @param password server password or \code{NULL}
#' @param use_ssl use secure connection to IRC server?
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
irc_server <- function(host_or_ip, port=6667, password=NULL, use_ssl=FALSE) {
  list(server=host_or_ip,
       port=port,
       server_password=password %||% "",
       use_ssl=use_ssl,
       nickname="RMsgBot",
       username="RMsgBot",
       realname="RMsgBot") -> irc_obj
  class(irc_obj) <- c("irc", class(irc_obj))
  irc_obj
}

#' Add nickname/user info
#'
#' You only need to use this function if you want to change the
#' nick/user/real names or need to authenticate the nick.
#'
#' @param nickname nickname (default is "\code{RMsgBot}")
#' @param username username (default is "\code{RMsgBot}")
#' @param realname realname (default is "\code{RMsgBot}")
#' @param password auth password for user (NOTE: not implemented yet)
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   post_message("Finishing a build is important, but building is more important.")
add_nick <- function(irc_obj, nickname="RMsgBot", username="RMsgBot",
                     realname="RMsgBot", password=NULL) {
  irc_obj$nickname <- nickname
  irc_obj$username <- username
  irc_obj$realname <- realname
  irc_obj
}

#' Set channel info
#'
#' @param channel channel to join on the IRC server
#' @param password channel password (NOTE: currently unused)
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
set_channel <- function(irc_obj, channel, password=NULL) {
  irc_obj$channel <- channel
  irc_obj$channel_password <- password
  irc_obj
}

#' Post an IRC message
#'
#' @param message character vector of messages to send
#' @note Due to inherent limitations in the IRC protocol, each message must not
#'       exceed 493 characters. Furthermore, since IRC messages are CRLF terminated,
#'       messages should not include these characters.
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
post_message <- function(irc_obj, message) {

  irc_notify_int(
    irc_server=irc_obj$server,
    port=as.integer(irc_obj$port),
    server_password=irc_obj$server_password,
    ssl=irc_obj$use_ssl,
    nickname=irc_obj$nickname,
    username=irc_obj$username,
    realname=irc_obj$realname,
    channel=irc_obj$channel,
    message=message
  )

}

#' Print method for IRC objects
#'
#' NOT IMPLEMENTED YET
#'
#' @export
print.irc <- function(x, ...) {
  print(str(x))
}