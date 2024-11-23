# ITTY & PCNTL

These two programs are for use with the [Internet Teletype](https://rtty.com/itty/index.htm) system to receive the audio stream continuously and output it to other tools that can then decode the stream, produce the text and send it to a real Teletype for printing.

The `itty` program receives the audio stream and attempts to keep the connection running. I wrote this because I thought there was a problem with `curl`, but it turned out that `curl -N -sS` works just fine, however I am keeping the code for posterity.

The `pcntl` program is used to power up and power down a Teletype between messages. This is to avoid leaving the Teletype switched on for long periods.

The code has only been written and tested on a Raspberry Pi running Raspbian GNU/Linux 12 (bookworm). The `pcntl` program has been written for a [BC Robotics Relay HAT](https://bc-robotics.com/shop/raspberry-pi-4-channel-relay-hat/), although it can be easily adapted to other Raspberry Pi HATs.

## Building

The `itty` code is based on [libcurl](https://curl.se/libcurl/). Accordingly the following packages must be installed:

```
sudo apt install libcurl4-gnutls-dev
```

The `pcntl` code requires WiringPi. This must be compiled by executing the following commands:

```
git clone  git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
sudo apt install ./
sudo apt install ./debian-template/wiringpi_3.10_armhf.deb
```

There is a simple makefile, so you just need to type:

```
make
```

This produces executables `itty` and `pcntl` in the same directory.

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
alias itty='itty http://internet-tty.net:8000/ITTY | lame --decode --quiet --mp3input - - | minimodem rtty -i -a -q --rx -R 11025 -f -'
```

You can use `curl -N -sS` in place of `itty` in the aliases above.

The EUROPE and ITTY feeds continuously send messages, but AUTOSTART only sends them intermittently.
So if you have using AUTOSTART and you want your Teletype (or any other device) to be powered on only when a message arrives,
then you should pipe the minimodem output into `pcntl`. `pcntl` takes the following options:

-d n The power on delay in seconds (default=5). If the power is off it powers on the device and waits n seconds before sending the first character. This is designed to give the equipment time to start.
-w n The power off delay in seconds (default=30). After the last data has been received it waits n seconds for more data and if there isn't any it switches off the device.
-r n The number of the relay on the BC Robotics Relay HAT.

An example is:

```
itty http://internet-tty.net:8030/AUTOSTART | lame --decode --quiet --mp3input - - | minimodem rtty -i -a -q -R 11025 -f - | pcntl -r 1
```
