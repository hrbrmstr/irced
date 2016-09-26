
<img src="wingr.png" width="25%"/>

------------------------------------------------------------------------

`irced` : \\ˈɜrkt\\

verb

1.  What users in your IRC channels will become if you send too many automated messages.

------------------------------------------------------------------------

This is an Rcpp/C++11 package for hooking R up to the world of Internet Relay Chat (IRC).

It's uses [`libircclient`](http://www.ulduzsoft.com/libircclient/index.html) which is bundled in with the package.

Build systems such as 'Travic-CI' have many ways to notify users when jobs have completed including posting a message to an IRC channel. In similar form, there are many times when analyses or data cleaning/processing operations are long running tasks that operate in the background or on remote systems. Functions are provided to notify users of the status of these tasks via IRC.

Eventually, it will be possible to make a complete IRC bot in R, but that's a future feature add.

If you're looking to host your own IRC server, I've found [`ngircd`](https://ngircd.barton.de) to be compact and easy to install/administer.

The API is is not set in stone as of yet so if you have any feature/API requests, please file an issue. This also means the API may change radically and break stuff.

Many thanks to @jeroenooms for his anticonf pattern/source.

The following functions are implemented:

-   `add_nick`: Add nickname/user info
-   `irced`: Putting the R into IRC
-   `irc_connect`: Create an IRC server object
-   `post_message`: Post an IRC message
-   `set_channel`: Set channel info

### TODO

-   \[x\] Bundle in `libircclient`
-   \[ \] Finish Windows port
-   \[x\] Separate out IRC server connection bits from the message sending bits (to support piping)
-   \[x\] Finish code for channel authentication
-   \[X\] Finish code/docs for SASL authentication
-   \[ \] Finish code for user authentication
-   \[ \] Add code to support writing an IRC bot in R

### Installation

``` r
devtools::install_github("hrbrmstr/irced")
```

### Working on Bots (subject to change)

``` r
    library(irced)
    library(jsonlite)

    # probably should/will turn this into an S4 or reference class
    my_bot <- function(event, origin, params) {
      
      # pretty event console logging
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
      
      if (event == "CHANNEL" & origin != "keryx") {
        if (params[2] == ">>QUIT") {
          bot_cmd_part(params[1])
        } else {
          res <- sample(c(FALSE, TRUE), 1)
          if (res) {
            bot_cmd_list(params[1])
          } else {
            bot_cmd_msg(params[1], sprintf("Something  %d", sample(100, 1)))
          }
        }
      }
      
    }

    irc_connect("irc.rud.is") %>% 
      set_channel("#builds") %>% 
      start_bot("my_bot")
```

### Notifications (subject to change)

``` r
library(irced)

# current verison
packageVersion("irced")
```

``` r
irc_connect("irc.rud.is") %>% 
 set_channel("#builds") %>% 
 post_message("Finishing a build is important, but building is more important.")
```

<!-- ### Test Results -->
