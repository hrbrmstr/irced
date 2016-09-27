#' Create an IRC server object
#'
#' @param host_or_ip IRC server FQDN or IP address
#' @param port IRC server port
#' @param password server password or \code{NULL}. If you are required to
#'     do \href{https://freenode.net/kb/answer/sasl}{SASL} authentication then
#'     this should be "\code{nick}:\code{password}". Be wary of using any
#'     authentication scheme without an SSL connection to the IRC server.
#' @param use_ssl use secure connection to IRC server?
#' @param verbose produce \emph{very} chatty IRC diagnostic messages?
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   irc_connect() -> session
irc_server <- function(host_or_ip, port=6667, password=NULL, use_ssl=FALSE, verbose=TRUE) {
  list(server=host_or_ip,
       port=port,
       server_password=password %||% "",
       use_ssl=use_ssl,
       session=NULL,
       verbose=verbose,
       nickname="keryx",
       username="keryx",
       realname="keryx") -> irc_obj
  class(irc_obj) <- c("irc", class(irc_obj))
  irc_obj
}

#' Add nickname/user info
#'
#' You only need to use this function if you want to change the
#' nick/user/real names or need to authenticate the nick. The default
#' nickmame (et al) is \href{https://www.britannica.com/topic/keryx}{keryx}.
#'
#' @param irc_obj irc object
#' @param nickname nickname (default is "\code{keryx}")
#' @param username username (default is "\code{keryx}")
#' @param realname realname (default is "\code{keryx}")
#' @param password auth password for user \emph{(NOTE: not implemented yet)}.
#'     If your server requires \href{https://freenode.net/kb/answer/sasl}{SASL}
#'     authentication then specify this information in the call to
#'     \code{irc_connect()} as noted in the help for that function. Use this
#'     method for the \code{/msg NickServ IDENTIFY foo password} method of
#'     user authentication to IRC. Be wary of using any authentication
#'     scheme without an SSL connection to the IRC server.
#'
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   irc_connect() -> session
add_nick <- function(irc_obj, nickname="keryx", username="keryx",
                     realname="keryx", password=NULL) {
  irc_obj$nickname <- nickname
  irc_obj$username <- username
  irc_obj$realname <- realname
  irc_obj
}

#' Make connection
#'
#' @param irc_obj irc object
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   irc_connect() -> session
irc_connect <- function(irc_obj) {

  irc_obj$session <- connect_irc(irc_obj$server, irc_obj$port, irc_obj$server_password,
                                 irc_obj$use_ssl,
                                 irc_obj$nickname, irc_obj$username, irc_obj$realname)
  irc_obj

}


#' Make bot connection
#'
#' @param irc_obj irc object
#' @export
#' @examples
#' irc_server("irc.rud.is") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   bot_connect() -> session
irc_connect <- function(irc_obj) {

  irc_obj$session <- bot_connect_int(irc_obj$server, irc_obj$port, irc_obj$server_password,
                                     irc_obj$use_ssl,
                                     irc_obj$nickname, irc_obj$username, irc_obj$realname)
  irc_obj

}


#' Disconnect
#'
#' @param irc_obj irc object
#' @export
irc_disconnect <- function(irc_obj) {
  disconnect_irc(irc_obj$session)
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
#'   post_message("#builds", "Finishing a build is important, but building is more important.")
post_message <- function(irc_obj, channel, message, channel_password=NULL) {

  if (irc_obj$use_ssl) message("SSL connections may take much longer than normal ones.")

  irc_post_message(
    irc_obj$server,
    port=as.integer(irc_obj$port),
    irc_obj$server_password,
    irc_obj$use_ssl,
    irc_obj$nickname,
    irc_obj$username,
    irc_obj$realname,
    channel,
    message,
    channel_password %||% ""
  )

}



#' Bot
#' @export
start_bot <- function(irc_obj, handler) {

  if (irc_obj$use_ssl) message("SSL connections may take much longer than normal ones.")

  irc_bot(
    irc_obj$session,
    irc_server=irc_obj$server,
    server_password=irc_obj$server_password,
    port=as.integer(irc_obj$port),
    ssl=irc_obj$use_ssl,
    nickname=irc_obj$nickname,
    username=irc_obj$username,
    realname=irc_obj$realname,
    channel=irc_obj$channel,
    channel_password=irc_obj$channel_password,
    verbose=irc_obj$verbose,
    bot_func=handler
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