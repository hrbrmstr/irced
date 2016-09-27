#include <Rcpp.h>

using namespace Rcpp;

#include <libircclient.h>
#include "irced.h"

#include <string.h>

#include <chrono>
#include <thread>


struct node {
  IRC irc ;
  node *next;
};

node irc_list = *(new node);
node *lstail = &irc_list;

void dump_event(irc_session_t *session, const char *event, const char *origin,
                const char **params, unsigned int count) {

  char buf[512];
  int cnt;

  buf[0] = '\0';

  for (cnt = 0; cnt < count; cnt++) {
    if (cnt) strcat (buf, "|");
    strcat (buf, params[cnt]);
  }

  Rcout << event << " : " << origin << " : " << buf << std::endl;

}

void event_channel(irc_session_t *session, const char *event, const char *origin,
                   const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(params[1]) == "QUIT") {
    irc_cmd_quit(session, "FINISHED");
  }

}

void event_part(irc_session_t *session, const char *event, const char *origin,
                const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == obj.get_nickname()) {
    irc_cmd_quit(session, 0);
  }

}

void event_join(irc_session_t *session, const char *event, const char *origin,
                const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) Rcout << event << " [[" << origin << "]] [[" << obj.get_nickname() << "]] " << count << std::endl;

  if (std::string(origin) == obj.get_nickname()) {

    for (auto msg : obj.get_message()) {
      irc_cmd_msg(session, obj.get_channel().c_str(),
                  as<std::string>(msg).substr(0, 493).c_str());
    }

    irc_cmd_part(session, obj.get_channel().c_str());

  }

}

void event_connect(irc_session_t *session, const char *event, const char *origin,
                   const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) Rcout << event << " " << origin << " " << count << std::endl;

  irc_cmd_join(session, obj.get_channel().c_str(),
               obj.get_channel_password() == "" ? obj.get_channel_password().c_str() : 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

}

void event_numeric (irc_session_t *session, unsigned int event, const char *origin,
                    const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) {

    char buf[512];
    int cnt;

    buf[0] = '\0';

    for (cnt = 0; cnt < count; cnt++) {
      if (cnt) strcat (buf, "|");
      strcat (buf, params[cnt]);
    }

    Rcout << event << " : " << origin << " : " << buf << std::endl;

  }

}

// [[Rcpp::export]]
void irc_post_message(std::string server, int port, std::string server_password,
                      bool ssl, std::string nick, std::string user, std::string real,
                      std::string channel,
                      CharacterVector message,
                      std::string channel_password) {

  IRC irc = IRC();

  irc.set_server(server, port, server_password, ssl);
  irc.set_nick(nick, user, real);
  irc.set_channel(channel, channel_password);

  irc.set_message(message);

  irc.set_event_handler("connect", event_connect);
  irc.set_event_handler("join", event_join);
  irc.set_event_handler("part", event_part);
  irc.set_event_handler("numeric", event_numeric);

  irc.create_session();
  irc.connect();
  irc.run();
  irc.disconnect();

}

// [[Rcpp::export]]
void disconnect_irc(Rcpp::XPtr<IRC> irc) {
  if (irc) irc->disconnect();
}

// [[Rcpp::export]]
Rcpp::XPtr<IRC> connect_irc(std::string server, int port, std::string server_password,
                            bool ssl, std::string nick, std::string user, std::string real) {

  IRC irc = IRC();

  irc.set_server(server, port, server_password, ssl);
  irc.set_nick(nick, user, real);

  irc.set_event_handler("connect", event_connect);
  irc.set_event_handler("join", event_join);
  irc.set_event_handler("part", event_part);
  irc.set_event_handler("numeric", event_numeric);

  irc.create_session();
  irc.connect();

  lstail->irc = irc;
  IRC *ptr = &lstail->irc;
  lstail-> next = new node;
  lstail = lstail->next;

  return(Rcpp::XPtr<IRC>(ptr));

}

/* BOT STUFF */

void bot_numeric(irc_session_t *session, unsigned int event, const char *origin,
                 const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) {
    char buf[512];
    buf[0] = '\0';
    for (int cnt=0; cnt<count; cnt++) {
      if (cnt) strcat (buf, "|");
      strcat (buf, params[cnt]);
    }
    Rcout << "BOT " << event << " : " << origin << " : " << buf << std::endl;
  }

  obj.call_bot_func((char *)std::to_string(event).c_str(), (char *)origin, (char **)params, count);

}

void bot_event(irc_session_t *session, const char *event, const char *origin,
               const char **params, unsigned int count) {

  IRC obj = *(IRC *)irc_get_ctx(session);

  if (obj.be_verbose()) {
    char buf[512];
    buf[0] = '\0';
    for (int cnt=0; cnt<count; cnt++) {
      if (cnt) strcat (buf, "|");
      strcat (buf, params[cnt]);
    }
    Rcout << "BOT " << event << " : " << origin << " : " << buf << std::endl;
  }

  obj.call_bot_func((char *)event, (char *)origin, (char **)params, count);

}

// [[Rcpp::export]]
void irc_bot(Rcpp::XPtr<IRC> irc,
             std::string server, int port, std::string server_password,
             bool ssl, std::string nick, std::string user, std::string real,
             std::string channel,
             CharacterVector message,
             std::string channel_password,
             std::string bot_func) {


  irc->set_channel(channel, channel_password);

  irc->set_bot_handler(bot_func);

  irc->create_session();
  irc->connect();
  irc->run();
  irc->disconnect();

}

// [[Rcpp::export]]
Rcpp::XPtr<IRC> bot_connect_int(std::string server, int port, std::string server_password,
                                bool ssl, std::string nick, std::string user, std::string real) {

  IRC irc = IRC();

  irc.set_server(server, port, server_password, ssl);
  irc.set_nick(nick, user, real);

  irc.set_all_handlers(bot_event, bot_numeric);

  irc.create_session();
  irc.connect();

  lstail->irc = irc;
  IRC *ptr = &lstail->irc;
  lstail-> next = new node;
  lstail = lstail->next;

  return(Rcpp::XPtr<IRC>(ptr));

}

//' quit
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_quit(Rcpp::XPtr<IRC> irc, std::string reason) {
  irc_cmd_quit(irc->get_session(), reason.c_str());
}

//' join
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_join(Rcpp::XPtr<IRC> irc, std::string channel, std::string password) {
  irc_cmd_join(irc->get_session(), channel.c_str(), password.c_str());
}

//' part
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_part(Rcpp::XPtr<IRC> irc, std::string channel) {
  irc_cmd_part(irc->get_session(), channel.c_str());
}

//' invite
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_invite(Rcpp::XPtr<IRC> irc, std::string nick, std::string channel) {
  irc_cmd_invite(irc->get_session(), nick.c_str(), channel.c_str());
}

//' names
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_names(Rcpp::XPtr<IRC> irc, std::string channel) {
  irc_cmd_names(irc->get_session(), channel.c_str());
}

//' list
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_list(Rcpp::XPtr<IRC> irc, std::string channel) {
  irc_cmd_list(irc->get_session(), channel.c_str());
}

//' topic
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_topic(Rcpp::XPtr<IRC> irc, std::string channel, std::string topic) {
  irc_cmd_topic(irc->get_session(), channel.c_str(), topic.c_str());
}

//' channel
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_channel(Rcpp::XPtr<IRC> irc, std::string channel, std::string mode) {
  irc_cmd_channel_mode(irc->get_session(), channel.c_str(), mode.c_str());
}

//' user
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_user(Rcpp::XPtr<IRC> irc, std::string mode) {
  irc_cmd_user_mode(irc->get_session(), mode.c_str());
}

//' nick
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_nick(Rcpp::XPtr<IRC> irc, std::string newnick) {
  irc_cmd_nick(irc->get_session(), newnick.c_str());
}

//' whois
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_whois(Rcpp::XPtr<IRC> irc, std::string nick) {
  irc_cmd_whois(irc->get_session(), nick.c_str());
}

//' msg
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_msg(Rcpp::XPtr<IRC> irc, std::string channel, std::string message) {
  irc_cmd_msg (irc->get_session(), channel.c_str(), message.c_str());
}

//' me
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_me(Rcpp::XPtr<IRC> irc, std::string channel, std::string message) {
  irc_cmd_me(irc->get_session(), channel.c_str(), message.c_str());
}

//' notice
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_notice(Rcpp::XPtr<IRC> irc, std::string channel, std::string message) {
  irc_cmd_notice(irc->get_session(), channel.c_str(), message.c_str());
}

//' kick
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_kick(Rcpp::XPtr<IRC> irc, std::string nick, std::string channel, std::string reason) {
  irc_cmd_kick(irc->get_session(), nick.c_str(), channel.c_str(), reason.c_str());
}



