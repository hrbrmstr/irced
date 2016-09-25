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
#' irc_connect("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
irc_connect <- function(host_or_ip, port=6667, password=NULL, use_ssl=FALSE,
                       verbose=FALSE) {
  list(server=host_or_ip,
       port=port,
       server_password=password %||% "",
       use_ssl=use_ssl,
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
#' irc_connect("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   add_nick("nick", "nick", "nick") %>%
#'   post_message("Finishing a build is important, but building is more important.")
add_nick <- function(irc_obj, nickname="keryx", username="keryx",
                     realname="keryx", password=NULL) {
  irc_obj$nickname <- nickname
  irc_obj$username <- username
  irc_obj$realname <- realname
  irc_obj
}

#' Set channel info
#'
#' @param channel channel to join on the IRC server
#' @param password channel password
#' @export
#' @examples
#' irc_connect("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
set_channel <- function(irc_obj, channel, password=NULL) {
  irc_obj$channel <- channel
  irc_obj$channel_password <- password %||% ""
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
#' irc_connect("irc.rud.is") %>%
#'   set_channel("#builds") %>%
#'   post_message("Finishing a build is important, but building is more important.")
post_message <- function(irc_obj, message) {

  if (irc_obj$use_ssl) message("SSL connections may take much longer than normal ones.")

  irc_notify_int(
    irc_server=irc_obj$server,
    server_password=irc_obj$server_password,
    port=as.integer(irc_obj$port),
    ssl=irc_obj$use_ssl,
    nickname=irc_obj$nickname,
    username=irc_obj$username,
    realname=irc_obj$realname,
    channel=irc_obj$channel,
    channel_password=irc_obj$channel_password,
    message=message,
    verbose=irc_obj$verbose
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