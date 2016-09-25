#' Putting the R into IRC
#'
#' Build systems such as 'Travic-CI' have many ways to notify
#' users when jobs have completed including posting a message to an IRC
#' channel. In similar form, there are many times when analyses
#' or data cleaning/processing operations are long running tasks that
#' operate in the background or on remote systems. Functions are provided
#' to notify users of the status of these tasks via IRC.
#'
#' @name irced
#' @docType package
#' @author Bob Rudis (@@hrbrmstr)
#' @import purrr
#' @useDynLib irced
#' @importFrom Rcpp sourceCpp
NULL
