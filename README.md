# Yee

This tiny utility can be used to connect arbitrary command line program to a network socket.
It does so by duplicating the defined file descriptor (by default 0) and launching given program with this fd attached to both stdin and stdout (and possibly stderr).

The name refers to the old UNIX utility `tee(1)` that takes one input and splits it to two outputs.
The schema of this (when drawn correctly) resembles a capital letter T.
Yee also takes one file descriptor, but splits it in a way that (when drawn correctly) resembles capital letter Y.

## Usage

Yee was initially written as an adaptor between `inetd(1)` (`launchd(1)` and `ssh(1)`).
Using yee, it is possible to launch ssh as a child of inted and pass it an opened socket file descriptor.
What's written to the socket will be passed to a remote computer, and the output will be returned back.

## Author

Vit Kabele <vit@kabele.me>

