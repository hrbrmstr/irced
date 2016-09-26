#include <Rcpp.h>

using namespace Rcpp;

#include <libircclient.h>

#include <string.h>

#include <chrono>
#include <thread>

CharacterVector g_message;
bool g_verbose;

std::string g_irc_server, g_server_password,
g_channel, g_channel_password,
g_nickname, g_username, g_realname, g_bot_func;

irc_session_t *g_session = NULL;

void dump_event(irc_session_t *session, const char *event, const char *origin,
                const char **params, unsigned int count) {

  if (g_verbose) {
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

void event_channel(irc_session_t *session, const char *event, const char *origin,
                   const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(params[1]) == "QUIT") {
    irc_cmd_quit(session, "FINISHED");
  }

}

void event_part (irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname) {
    irc_cmd_quit(session, 0);
  }

}

void event_join (irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname) {

    for (auto msg : g_message) {
      irc_cmd_msg(session, g_channel.c_str(), as<std::string>(msg).substr(0, 493).c_str());
    }

    irc_cmd_part(session, g_channel.c_str());

  }

}

void event_connect (irc_session_t *session, const char *event, const char *origin,
                    const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  irc_cmd_join(session, g_channel.c_str(),
               g_channel_password == "" ? g_channel_password.c_str() : 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

}

void event_numeric (irc_session_t *session, unsigned int event, const char *origin,
                    const char **params, unsigned int count) {

  if (g_verbose) {

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
void irc_notify_int(CharacterVector irc_server,
                    CharacterVector server_password,
                    CharacterVector channel,
                    CharacterVector channel_password,
                    CharacterVector message,
                    int port,
                    bool ssl,
                    CharacterVector nickname,
                    CharacterVector username,
                    CharacterVector realname,
                    bool verbose) {

  irc_callbacks_t callbacks;
  irc_session_t *session;

  g_irc_server = irc_server[0];
  g_server_password = server_password[0];

  g_channel = channel[0];
  g_channel_password = channel_password[0];

  g_message = message;
  g_nickname = nickname[0];
  g_username = username[0];
  g_realname = realname[0];

  g_verbose = verbose;

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
    if (g_verbose) Rcout << "Using SSL" << std::endl;
    g_irc_server = "#" + g_irc_server;
    irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);
  }

  if (irc_connect(session, g_irc_server.c_str(), port,
                  (g_server_password=="" ? 0 : g_server_password.c_str()),
                  g_nickname.c_str(), g_username.c_str(), g_realname.c_str())) {
  } else {
    if (g_verbose) Rcout << "Connected" << std::endl;
    irc_run(session);
  }

  if (g_verbose) Rcout << "DONE" << std::endl;
  irc_disconnect(session);
  if (g_verbose) Rcout << "CLOSED" << std::endl;

}

/* BOT STUFF */

void bot_event_quit(irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

}

void bot_event_error(irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

}

void bot_event_join(irc_session_t *session, const char *event, const char *origin,
              const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname) {

  } else {

    CharacterVector p(count);
    for (int cnt=0; cnt<count; cnt++) {
      p[cnt] = std::string(params[cnt]);
    }

    Environment env = Environment::global_env();
    Function f = env[g_bot_func];
    f(event, std::string(origin), p);

  }

}

void bot_event_part(irc_session_t *session, const char *event, const char *origin,
              const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  if (std::string(origin) == g_nickname) {

    irc_cmd_quit(g_session, 0);

  } else {

    CharacterVector p(count);
    for (int cnt=0; cnt<count; cnt++) {
      p[cnt] = std::string(params[cnt]);
    }

    Environment env = Environment::global_env();
    Function f = env[g_bot_func];
    f(event, std::string(origin), p);

  }

}

void bot_event_connect(irc_session_t *session, const char *event, const char *origin,
                 const char **params, unsigned int count) {

  if (g_verbose) Rcout << event << " " << origin << " " << count << std::endl;

  irc_cmd_join(session, g_channel.c_str(),
               g_channel_password == "" ? g_channel_password.c_str() : 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

}

void bot_event_numeric (irc_session_t *session, unsigned int event, const char *origin,
                  const char **params, unsigned int count) {

  if (g_verbose) {

    char buf[512];
    int cnt;

    buf[0] = '\0';

    for (cnt = 0; cnt < count; cnt++) {
      if (cnt) strcat (buf, "|");
      strcat (buf, params[cnt]);
    }

    Rcout << "BOT " << event << " : " << origin << " : " << buf << std::endl;

  }

  CharacterVector p(count);
  for (int cnt=0; cnt<count; cnt++) {
    p[cnt] = std::string(params[cnt]);
  }

  Environment env = Environment::global_env();
  Function f = env[g_bot_func];
  f(std::to_string(event), std::string(origin), p);

}

void bot_event(irc_session_t *session, const char *event, const char *origin,
               const char **params, unsigned int count) {

  if (g_verbose) {
    char buf[512];

    buf[0] = '\0';

    for (int cnt=0; cnt<count; cnt++) {
      if (cnt) strcat (buf, "|");
      strcat (buf, params[cnt]);
    }

    Rcout << "BOT " << event << " : " << origin << " : " << buf << std::endl;
  }

  CharacterVector p(count);
  for (int cnt=0; cnt<count; cnt++) {
    p[cnt] = std::string(params[cnt]);
  }

  Environment env = Environment::global_env();
  Function f = env[g_bot_func];
  f(std::string(event), std::string(origin), p);

}

// [[Rcpp::export]]
void irc_bot(CharacterVector irc_server,
             CharacterVector server_password,
             CharacterVector channel,
             CharacterVector channel_password,
             int port,
             bool ssl,
             CharacterVector nickname,
             CharacterVector username,
             CharacterVector realname,
             bool verbose,
             std::string bot_func) {

  CharacterVector message("Bollocks");

  g_bot_func = bot_func;

  irc_callbacks_t callbacks;
  irc_session_t *session;

  g_irc_server = irc_server[0];
  g_server_password = server_password[0];

  g_channel = channel[0];
  g_channel_password = channel_password[0];

  g_message = message;
  g_nickname = nickname[0];
  g_username = username[0];
  g_realname = realname[0];

  g_verbose = verbose;

  memset(&callbacks, 0, sizeof(callbacks));

  callbacks.event_connect = bot_event_connect;
  callbacks.event_nick = bot_event;
  callbacks.event_quit = bot_event_quit;
  callbacks.event_join = bot_event_join;
  callbacks.event_part = bot_event_part;
  callbacks.event_mode = bot_event;
  callbacks.event_umode = bot_event;
  callbacks.event_topic = bot_event;
  callbacks.event_kick = bot_event;
  callbacks.event_channel = bot_event;
  callbacks.event_privmsg = bot_event;
  callbacks.event_notice = bot_event;
  callbacks.event_invite = bot_event;
  callbacks.event_umode = bot_event;
  callbacks.event_ctcp_rep = bot_event;
  callbacks.event_ctcp_action = bot_event;
  callbacks.event_unknown = bot_event_error;
  callbacks.event_numeric = bot_event_numeric;

  session = irc_create_session(&callbacks);
  g_session = session;

  irc_option_set(session, LIBIRC_OPTION_STRIPNICKS);

  if (ssl) {
    if (g_verbose) Rcout << "Using SSL" << std::endl;
    g_irc_server = "#" + g_irc_server;
    irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);
  }

  if (irc_connect(session, g_irc_server.c_str(), port,
                  (g_server_password=="" ? 0 : g_server_password.c_str()),
                  g_nickname.c_str(), g_username.c_str(), g_realname.c_str())) {
  } else {
    if (g_verbose) Rcout << "Connected" << std::endl;
    irc_run(g_session);
  }

  if (g_verbose) Rcout << "DONE" << std::endl;
  irc_disconnect(g_session);
  if (g_verbose) Rcout << "CLOSED" << std::endl;

}


//' quit
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_quit(std::string reason) {
  irc_cmd_quit(g_session, reason.c_str());
}

//' join
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_join(std::string channel, std::string password) {
  irc_cmd_join(g_session, channel.c_str(), password.c_str());
}

//' part
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_part(std::string channel) {
  irc_cmd_part(g_session, channel.c_str());
}

//' invite
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_invite(std::string nick, std::string channel) {
  irc_cmd_invite(g_session, nick.c_str(), channel.c_str());
}

//' names
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_names(std::string channel) {
  irc_cmd_names(g_session, channel.c_str());
}

//' list
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_list(std::string channel) {
  irc_cmd_list(g_session, channel.c_str());
}

//' topic
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_topic(std::string channel, std::string topic) {
  irc_cmd_topic(g_session, channel.c_str(), topic.c_str());
}

//' channel
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_channel(std::string channel, std::string mode) {
  irc_cmd_channel_mode(g_session, channel.c_str(), mode.c_str());
}

//' user
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_user(std::string mode) {
  irc_cmd_user_mode(g_session, mode.c_str());
}

//' nick
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_nick(std::string newnick) {
  irc_cmd_nick(g_session, newnick.c_str());
}

//' whois
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_whois(std::string nick) {
  irc_cmd_whois(g_session, nick.c_str());
}

//' msg
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_msg(std::string channel, std::string message) {
  irc_cmd_msg (g_session, channel.c_str(), message.c_str());
}

//' me
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_me(std::string channel, std::string message) {
  irc_cmd_me(g_session, channel.c_str(), message.c_str());
}

//' notice
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_notice(std::string channel, std::string message) {
  irc_cmd_notice(g_session, channel.c_str(), message.c_str());
}

//' kick
//'
//' @export
// [[Rcpp::export]]
void bot_cmd_kick(std::string nick, std::string channel, std::string reason) {
  irc_cmd_kick(g_session, nick.c_str(), channel.c_str(), reason.c_str());
}

