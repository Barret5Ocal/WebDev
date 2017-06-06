HOST = "irc.twitch.tv"                              # the Twitch IRC server
PORT = 6667                                         # always use port 6667!
NICK = "barret5ocal_tog_dev"                        # your Twitch username, lowercase
PASS = "oauth:0qo03hcn20kfo2wkjlrgw0f6hxzdtx"       # your Twitch OAuth token
CHAN = "#channel"                                   # the channel you want to join
RATE = (20/30)                                      # messages per second

# Make sure you prefix the quotes with an 'r'!
CHAT_MSG=re.compile(r"^:\w+!\w+@\w+\.tmi\.twitch\.tv PRIVMSG #\w+ :")

import socket
import re
import time

def chat(sock, msg):
	"""
	Send a chat message to the server.
	Keyword arguments:
	sock -- the socket over which to send the message
	msg  -- the message to be sent
	"""
	sock.send("PRIVMSG #{} :{}".format(cfg.CHAN, msg))


print("PASS {}\r\n".format(cfg.PASS).encode("utf-8"))
print("NICK {}\r\n".format(cfg.NICK).encode("utf-8"))
print("JOIN {}\r\n".format(cfg.CHAN).encode("utf-8"))

s = socket.socket()
s.connect((cfg.HOST, cfg.PORT))
s.send("PASS {}\r\n".format(cfg.PASS).encode("utf-8"))
s.send("NICK {}\r\n".format(cfg.NICK).encode("utf-8"))

s.send("JOIN {}\r\n".format(cfg.CHAN).encode("utf-8"))

while True:
	response = s.recv(1024).decode("utf-8")
	if response == "PING :tmi.twitch.tv\r\n":
		s.send("PONG :tmi.twitch.tv\r\n".encode("utf-8"))
		print("PING\r\n".encode("utf-8"))
	else:
		username = re.search(r"\w+", response).group(0) # return the entire match
		message = CHAT_MSG.sub("", response)
		print(username + ": " + message)
		for pattern in cfg.PATT:
			if re.match(pattern, message):
				 
		time.sleep(1 / cfg.RATE)

