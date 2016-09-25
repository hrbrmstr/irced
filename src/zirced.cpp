#include <Rcpp.h>

using namespace Rcpp;

#include <libircclient.h>

#include <string.h>

#include <chrono>
#include <thread>

std::string g_channel;
std::string g_nickname;
std::string g_username;
std::string g_realname;

CharacterVector g_message;

void dump_event(irc_session_t *session, const char *event, const char *origin,
                const char **params, unsigned int count) {

  char buf[512];
  int cnt;

  buf[0] = '\0';

  for (cnt = 0; cnt < count; cnt++) {
    if (cnt) strcat (buf, "|");
    strcat (buf, params[cnt]);
  }

  // Rcout << event << " : " << buf << std::endl;

}


void event_channel(irc_session_t *session, const char *event, const char *origin,
                   const char **params, unsigned int count) {

  // Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(params[1]) == "QUIT") {
    irc_cmd_quit(session, "FINISHED");
  }

}

void event_part (irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  // Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname.c_str()) {
    irc_cmd_quit(session, 0);
  }

}

void event_join (irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  // Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname.c_str()) {

    for (auto msg : g_message) {
      irc_cmd_msg(session, g_channel.c_str(), as<std::string>(msg).substr(0, 493).c_str());
    }

    irc_cmd_part(session, g_channel.c_str());

  }

}

void event_connect (irc_session_t *session, const char *event, const char *origin,
                    const char **params, unsigned int count) {

  // Rcout << event << " " << origin << " " << count << std::endl;

  irc_cmd_join(session, g_channel.c_str(), 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

}

void event_numeric (irc_session_t *session, unsigned int event, const char *origin,
                    const char **params, unsigned int count) {
  // Rcout << event << " " << origin << " " << count << std::endl;
}

//' Send a message/notification to an IRC server/channel
//'
//' Given an IRC server and port, plus channel and character vector,
//' send the contents of the character vector as separate messages to
//' the specified channel.
//'
//' @param irc_server IRC server FQDN or IP address
//' @param port IRC server port
//' @param server_password server password or \code{NULL}
//' @param nickname nickname (default is "\code{RMsgBot}")
//' @param username username (default is "\code{RMsgBot}")
//' @param realname realname (default is "\code{RMsgBot}")
//' @param channel channel on the IRC server
//' @param message character vector of messages to send
//' @param ssl use secure connection to IRC server?
//' @note Due to inherent limitations in the IRC protocol, each message must not
//'       exceed 493 characters. Furthermore, since IRC messages are CRLF terminated,
//'       messages should not include these characters.
//' @export
//' @examples
//' irc_notify("irc.rud.is", "#builds", "test")
// [[Rcpp::export]]
void irc_notify(std::string irc_server,
                std::string channel="#",
                CharacterVector message="",
                int port=6667,
                bool ssl=false,
                std::string server_password="",
                std::string nickname="RMsgBot",
                std::string username="RMsgBot",
                std::string realname="RMsgBot") {

  irc_callbacks_t callbacks;
  irc_session_t *session;

  g_channel = channel;
  g_message = message;
  g_nickname = nickname;
  g_username = username;
  g_realname = realname;

  memset(&callbacks, 0, sizeof(callbacks));

  callbacks.event_connect = event_connect;
  callbacks.event_numeric = event_numeric;
  callbacks.event_join = event_join;
  callbacks.event_nick = dump_event;
  callbacks.event_quit = dump_event;
  callbacks.event_part = event_part;
  callbacks.event_mode = dump_event;
  callbacks.event_topic = dump_event;
  callbacks.event_kick = dump_event;
  callbacks.event_channel = event_channel;
  callbacks.event_privmsg = dump_event;
  callbacks.event_notice = dump_event;
  callbacks.event_invite = dump_event;
  callbacks.event_umode = dump_event;
  callbacks.event_ctcp_rep = dump_event;
  callbacks.event_ctcp_action = dump_event;
  callbacks.event_unknown = dump_event;

  session = irc_create_session(&callbacks);

  irc_option_set(session, LIBIRC_OPTION_STRIPNICKS);

  if (ssl) {
    irc_server = "#" + irc_server;
    irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);
  }

  if (irc_connect(session, irc_server.c_str(), port,
                  (server_password=="" ? 0 : server_password.c_str()),
                  g_nickname.c_str(), g_username.c_str(), g_realname.c_str())) {
  } else {
    // Rcout << "Connected" << std::endl;
    irc_run(session);
  }

  // Rcout << "DONE" << std::endl;
  irc_disconnect(session);
  // Rcout << "CLOSED" << std::endl;

}
