
------------------------------------------------------------------------

`irced` :  

verb

1.  What users in your IRC channels will become if you send too many automated messages.

------------------------------------------------------------------------

This is an Rcpp/C++11 package for hooking R up to the world of Internet Relay Chat (IRC).

It's uses [`libircclient`](http://www.ulduzsoft.com/libircclient/index.html) which is bundled in with the package.

Build systems such as 'Travic-CI' have many ways to notify users when jobs have completed including posting a message to an IRC channel. In similar form, there are many times when analyses or data cleaning/processing operations are long running tasks that operate in the background or on remote systems. Functions are provided to notify users of the status of these tasks via IRC.

If you're looking to host your own IRC server, I've found [`ngircd`](https://ngircd.barton.de) to be compact and easy to install/administer.

The API is no doubt going to radically change to support the TODOs below. If you have any feature/API requests, please file an issue.

The package is titled as it is b/c if you send too many automated messages to an IRC channel with others in said channel this is what the'll become.

Many thanks to @jeroenooms for his anticonf pattern/source.

The following functions are implemented:

-   `add_nick`: Add nickname/user info
-   `irced`: Putting the R into IRC
-   `irc_server`: Create an IRC server object
-   `post_message`: Post an IRC message
-   `set_channel`: Set channel info

### TODO

-   \[x\] Bundle in `libircclient`
-   \[ \] Finish Windows port
-   \[x\] Separate out IRC server connection bits from the message sending bits (to support piping)
-   \[ \] Add a way to provide user authentication

### Installation

``` r
devtools::install_github("hrbrmstr/irced")
```

### Usage

``` r
library(irced)

# current verison
packageVersion("irced")
```

    ## [1] '0.1.0'

``` r
irc_server("irc.rud.is") %>% 
 set_channel("#builds") %>% 
 post_message("Finishing a build is important, but building is more important.")
```

### Test Results

``` r
library(irced)
library(testthat)

date()
```

    ## [1] "Sun Sep 25 09:17:30 2016"

``` r
test_dir("tests/")
```

    ## testthat results ========================================================================================================
    ## OK: 0 SKIPPED: 0 FAILED: 0
    ## 
    ## DONE ===================================================================================================================
