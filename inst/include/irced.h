#include <Rcpp.h>

#include "libircclient.h"

using namespace Rcpp;

#include <string.h>

#include <chrono>
#include <thread>

#ifndef __IRCED_CORE__
#define __IRCED_CORE__

class IRC {

public:

  IRC() {
    session = NULL;
    verbose = true;
    clear_handlers();

  }

  void set_server(std::string server, int irc_port, std::string password, bool use_ssl) {
    irc_server = server;
    port = irc_port;
    server_password = password;
    ssl = use_ssl;
  }

  std::string get_server() { return(irc_server); }

  void set_verbose() { verbose = true; }

  bool be_verbose() { return(verbose); }

  void set_channel(std::string name, std::string password) {
    channel = name;
    channel_password = password;
  }

  std::string get_channel() { return(channel); }
  std::string get_channel_password() { return(channel_password); }

  void set_nick(std::string nick, std::string user, std::string real) {
    nickname = nick;
    username = user;
    realname = real;
  }

  std::string get_nickname() { return(nickname); }
  std::string get_username() { return(username); }
  std::string get_realname() { return(realname); }

  void set_message(CharacterVector msg) { message = msg; }

  CharacterVector get_message() { return(message); }

  void create_session() {

    session = irc_create_session(&callbacks);

    irc_set_ctx(session, this);

    irc_option_set(session, LIBIRC_OPTION_STRIPNICKS);

    if (ssl) {
      if (verbose) Rcout << "Using SSL" << std::endl;
      irc_server = "#" + irc_server;
      irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);
    }

  }

  irc_session_t *get_session() { return(session); }

  void clear_handlers() {
    memset(&callbacks, 0, sizeof(callbacks));
  }

  void set_event_handler(std::string name, irc_eventcode_callback_t handler) {
    if (name == "numeric") callbacks.event_numeric = handler;
  }

  void set_all_handlers(irc_event_callback_t h1, irc_eventcode_callback_t h2) {

    clear_handlers();

    callbacks.event_connect = h1;
    callbacks.event_nick = h1;
    callbacks.event_quit = h1;
    callbacks.event_join = h1;
    callbacks.event_part = h1;
    callbacks.event_mode = h1;
    callbacks.event_umode = h1;
    callbacks.event_topic = h1;
    callbacks.event_kick = h1;
    callbacks.event_channel = h1;
    callbacks.event_privmsg = h1;
    callbacks.event_notice = h1;
    callbacks.event_invite = h1;
    callbacks.event_umode = h1;
    callbacks.event_ctcp_rep = h1;
    callbacks.event_numeric = h2;

  }

  void set_event_handler(std::string name, irc_event_callback_t handler) {

    if (name == "connect") callbacks.event_connect = handler;
    if (name == "nick") callbacks.event_nick = handler;
    if (name == "quit") callbacks.event_quit = handler;
    if (name == "join") callbacks.event_join = handler;
    if (name == "part") callbacks.event_part = handler;
    if (name == "mode") callbacks.event_mode = handler;
    if (name == "umode") callbacks.event_umode = handler;
    if (name == "topic") callbacks.event_topic = handler;
    if (name == "kick") callbacks.event_kick = handler;
    if (name == "channel") callbacks.event_channel = handler;
    if (name == "privmsg") callbacks.event_privmsg = handler;
    if (name == "notice") callbacks.event_notice = handler;
    if (name == "invite") callbacks.event_invite = handler;
    if (name == "umode") callbacks.event_umode = handler;
    if (name == "rep") callbacks.event_ctcp_rep = handler;

  }

  void call_bot_func(Rcpp::XPtr<IRC> obj,
                     char *event, char *origin, char **params,
                     unsigned int count) {

    Rcout << "Calling bot function..." << std::endl;

    CharacterVector p(count);
    for (int cnt=0; cnt<count; cnt++) {
      p[cnt] = std::string(params[cnt]);
    }

    Rcout << "in call_bot_func ==> " << event << " | " << origin << " | " << p << " | " << count << std::endl;

    bot_func(obj, std::string(event), std::string(origin), p, count);

  }

  void set_bot_handler(Function f) {
    bot_func = f;
  }

  void connect() {
    Rcout << "===> CONNECT" << std::endl;
    if (session) {
      int ret = irc_connect(session, irc_server.c_str(), port,
                            (server_password=="" ? 0 : server_password.c_str()),
                            nickname.c_str(), username.c_str(), realname.c_str());
      Rcout << "Connected " << ret << std::endl;
      IRC tmp = *(IRC *)irc_get_ctx(session);
      Rcout << "ctx test " << tmp.get_server() << std::endl;
    }
  }

  void disconnect() {
    Rcout << "===> DISCONNECT" << std::endl;
    if (session) {
      irc_disconnect(session);
      Rcout << "Disconnected" << std::endl;
    }
  }

  void run() {
    Rcout << "===> RUN" << std::endl;
    if (session) irc_run(session);
  }

private:

  CharacterVector message;

  irc_callbacks_t callbacks;
  irc_session_t *session;

  bool verbose;
  bool ssl;

  int port;

  std::string irc_server;
  std::string server_password;
  std::string channel;
  std::string channel_password;
  std::string nickname;
  std::string username;
  std::string realname;

  Function bot_func = Environment::base_env()["cat"];

};

#endif // __IRCED_CORE__