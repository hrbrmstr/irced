
`irced` :  : Putting the "R" in IRC

This is an Rcpp/C++11 package for hooking R up to the world of Internet Relay Chat (IRC).

It's uses [`libircclient`](http://www.ulduzsoft.com/libircclient/index.html) which is bundled in with the package.

The use-case for this is to notify you after a long running process is complete but the package will eventually have support for being an "R IRC bot" because the world desperately needs more IRC bots. Inspriation for it came from finding out that Travis-CI can notify you of builds on IRC.

I've found [`ngircd`](https://ngircd.barton.de) to be compact and easy to install/administer if you want to host your own IRC serer.

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

    ## [1] "Sun Sep 25 08:13:25 2016"

``` r
test_dir("tests/")
```

    ## testthat results ========================================================================================================
    ## OK: 0 SKIPPED: 0 FAILED: 0
    ## 
    ## DONE ===================================================================================================================
