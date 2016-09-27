#include <Rcpp.h>

#include "irced.h"

#ifndef __IRCED_TYPES__
#define __IRCED_TYPES__

void finaliseIRC(IRC *irc_inst){ delete irc_inst; }

typedef Rcpp::XPtr<IRC, Rcpp::PreserveStorage, finaliseIRC> XPtrIRC;

#endif // __IRCED_TYPES__
