
`irced` :  : Putting the "R" in IRC

This is an Rcpp/C++11 pacakge for hooking R up to the world of Internet Relay Chat (IRC).

It's uses [`libircclient`](http://www.ulduzsoft.com/libircclient/index.html) which will eventually be bundled in with the pacakge but, for now, requires you to install `libircclient` separately **and** with `openssl` support **and** with `libircclient.a` in `/usr/local/lib/` (or, you'll have to edit `Makevars`). Yes, that's annoying, but it's going to be bundled with the pacakge soon, so it's only for those folks who are desperate to spam your IRC friends with messages from R.

The use-case for this is to notify you after a long running process is complete but the pacakge will eventually have support for being an "R IRC bot" because the world desperately needs more IRC bots. Inspriation for it came from finding out that Travis-CI can notify you of builds on IRC.

The API is no doubt going to radically change to support the TODOs below. You've been warned.

The following functions are implemented:

-   `irc_notify` : Send messages/notifications to an IRC sever+channel

### TODO

-   \[ \] Bundle in `libircclient`
-   \[ \] Separate out IRC server connection bits from the message sending bits (to support piping)
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
irc_notify("irc.rud.is", "#builds", "test")
```

### Test Results

``` r
library(irced)
library(testthat)

date()
```

    ## [1] "Sat Sep 24 16:18:42 2016"

``` r
test_dir("tests/")
```

    ## testthat results ========================================================================================================
    ## OK: 0 SKIPPED: 0 FAILED: 0
    ## 
    ## DONE ===================================================================================================================
