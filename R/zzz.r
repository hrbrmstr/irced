.onAttach <- function(...) {

  if (!interactive()) return()

  packageStartupMessage(paste0("irced is under *active* development. ",
                               "See https://github.com/hrbrmstr/irced for info/news."))

}

