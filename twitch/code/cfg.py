# cfg.py
HOST = "irc.twitch.tv"                              # the Twitch IRC server
PORT = 6667                                         # always use port 6667!
NICK = "barret5ocal_tog_dev"                        # your Twitch username, lowercase
PASS = "oauth:0qo03hcn20kfo2wkjlrgw0f6hxzdtx"       # your Twitch OAuth token
CHAN = "#channel"                                   # the channel you want to join
RATE = (20/30)                                      # messages per second
PATT = [
    r"swear",
    # ...
    r"some_pattern"
]