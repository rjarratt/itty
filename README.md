# ITTY

This program is for use with the [Internet Teletype](https://rtty.com/itty/index.htm) system to receive the audio stream continuously and output it to other tools that can then decode the stream and produce the text. This text can then be sent to a real Teletype for printing.

It is possible to use `curl -N` for this, but after each message it terminates. This code does not terminate and waits for the next stream to start without exiting, so that a continuous stream of messages can be produced.

The code has only been written and tested on a Raspberry Pi running Raspbian GNU/Linux 12 (bookworm).

## Building

The code is based on [libcurl](https://curl.se/libcurl/). Accordingly the following packages must be installed:

```
sudo apt install libcurl4-gnutls-dev
```

There is a simple makefile, so you just need to type:

```
make
```

This produced an executable `itty` in the same directory.

You can also use:

```
make install
```

To have the code built and installed in `/usr/local/bin`.

## Using

To be able to decode and output messages two utilities need to be installed:

```
sudo apt install lame
sudo apt install minimodem
```

The following aliases are adapted from some given to me by a member of the Greenkeys mailing list:

```
alias autostart='itty http://internet-tty.net:8030/AUTOSTART | lame --decode --quiet --mp3input - - | minimodem rtty -i -a -q -R 11025 -f -'
alias europe='itty internet-tty.net:8040/EUROPE | lame --decode --quiet --mp3input - - | minimodem 50 -M 1275 -S 1445 -5 -i -a -q -R 11025 -f -'
alias itty='itty http://internet-tty.net:8000/ITTY | lame --decode --quiet --mp3input - - | minimodem rtty -i -a -q --rx-one -R 11025 -f -'
```
