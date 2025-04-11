# wordle
 
### Get 6 chances to guess a 5-letter word.

wordle is a simple pure C project made for 42 Firenze.
42 is an international school of coding.

## functionality
Internally *wordle* use poll() on the server side to manage incoming sockets.
This program provides both server and client side functionality.
Client will send data formatted in a way it is readble by the server.

We also provide a very basic records system, using text files to preserve sessions.

## installation
this project provides a Makefile. Just launch make and everything is done!

## usage

### start server
  - wordle serve
this command will start the server on the localhost address

### signup
  - wordle signup username:password url
register an user to be able to play

### login
  - wordle login username:password url
login and have fun!
