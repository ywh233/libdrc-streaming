This is an experimental project to see if I can stream videos from a Windows
device to a Wii U gamepad through a proxy server running on a Raspberry Pi. The
end result is that it *sort of* works but **isn't practical** as the gamepad's
screen will freeze after streaming for ~5-10 seconds, thanks to the TSF drifting
issue of my Wifi adaptor.

The general idea is to run a proxy server on Raspberry Pi that accepts video
data (in this case, pre-encoded H264 chunks) from the client and sends it to a
modified libdrc streamer that accepts pre-encoded H264 chunks, then run a client
on the Windows PC to do the heavy encoding work and send the data to the proxy
server. The server/client were built with gRPC since I was too lazy to implement
a TCP server from the ground up :)

## Setup

### Hardware Requirements

1. A Wii U gamepad :)
1. A Raspberry Pi
1. A rt2800usb wifi adaptor. I was using Panda Wireless PAU09 N600
1. An ethernet cable to connect your Raspberry Pi with your Windows PC. Your
   Windows PC also needs an ethernet NIC.

### Detailed Steps

1.  Configure your Raspberry Pi to connect to your gamepad. I was basically
    following instructions from [this blog](https://eev.ee/blog/2015/11/28/zdoom-on-wii-u-gamepad-with-raspberry-pi/).
    * Use the modified libdrc repo that I forked instead of the vanilla one.
      It allows the caller to push encoded H264 data instead of the raw image
      data.
1.  Connect your Raspberry Pi to your PC with an ethernet cable, then give them
    static IP addresses in the same subnet. Make sure they can ping each other.
1.  Install MSYS2/MinGW64 on your Windows PC.
1.  Check out this repo on both your Windows PC and Raspberry Pi.
1.  Build drc-streamer with CMake on your Windows PC, with the platform set to
    MinGW.
1.  Build drc-proxy with CMake on your Raspberry Pi.
1.  Run the modified wpa_supplicant, hostapd, and the DHCP server (probably
    netboot) on your Raspberry Pi.
1.  Run drc-proxy on Raspberry Pi, then run drc-streamer on the Windows PC.
1.  Connect your Wii U gamepad to your Raspberry Pi.

### Result

If everything is set up correctly, you will see your gamepad's screen changing
color with a gradient animation. Unfortunately for me the video would soon start
stuttering and eventually freeze in a few seconds, it probably has to do with
the TSF drift issue of the Wifi NIC.

### Observations

1.  Software H264 encoding on Raspberry Pi 4 is pretty much unusable as the
    frame rate will drop to something like 2 fps.
1.  Button inputs from the gamepad stay working even after the screen is frozen,
    and the gamepad doesn't seem to turn itself off, unlike what would happen if
    you don't have a patched mac80211 module.

### Thoughts

Given libdrc is pretty much unmaintained, and the TSF feature is not quite being
used in normal situations and is going away with new Wifi NICs, I don't think
the TSF drift issue would ever improve unless someone manages to write a custom
driver that fixes it or somehow manipulate the link layer packet to fix the
broken TSF values. I'd be really appreciated if someone finds a way to
workaround it though :)

Observation#2 is interesting, as it sounds like we may build something like
UsendMii without requiring a Wii U console. This could be done by making
Raspberry Pi send input data to the Windows PC. The Pi may still need to send
empty frames to the gamepad just to prevent it from turning off. The downside
is that it's quite a hassle to set up the Pi, which could be streamlined a bit,
say if we make a disk image with everything set up.
