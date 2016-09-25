context("basic functionality")
test_that("we can do something", {

  irc_connect("irc.rud.is", password="Seekrit") %>%
    set_channel("#builds") %>%
    post_message("Finishing a build is important, but building is more important.")

  irc_connect("irc.rud.is", password="Seekrit") %>%
    set_channel("#Seekrit", "Seekrit") %>%
    post_message("Finishing a build is important, but building is more important.")

})
