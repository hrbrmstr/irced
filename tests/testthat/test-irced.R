context("basic functionality")
test_that("we can do something", {

  irc_server("irc.rud.is") %>%
    set_channel("#builds") %>%
    post_message("Finishing a build is important, but building is more important.")

})
