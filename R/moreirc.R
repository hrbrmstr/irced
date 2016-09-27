

# Bot
# @export
# start_bot <- function(irc_obj, handler) {
#
#   if (irc_obj$use_ssl) message("SSL connections may take much longer than normal ones.")
#
#   irc_bot(
#     irc_server=irc_obj$server,
#     server_password=irc_obj$server_password,
#     port=as.integer(irc_obj$port),
#     ssl=irc_obj$use_ssl,
#     nickname=irc_obj$nickname,
#     username=irc_obj$username,
#     realname=irc_obj$realname,
#     channel=irc_obj$channel,
#     channel_password=irc_obj$channel_password,
#     verbose=irc_obj$verbose,
#     bot_func=handler
#   )
#
# }
