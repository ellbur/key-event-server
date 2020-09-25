
Scripts for tapping into `/dev/uinput`

# Rationale

There is a delay involved in creating a keyboard device via `ioctl` to access `/dev/uinput`. Also, some software will not immediately see the device.

Therefore, for `xdotool`-like scripting, it is advantageous to run a server that accepts key presses.

This code uses a filesystem FIFO instead of a socket because that was a convenient way to limit access to only the current user.

A FIFO will be created in `~/.local/key-event-service/FIFO`. Writing a line to the FIFO in JSON format will cause key presses to be sent.

The JSON format is:

    [ {"code": ..., "value": ...}, ... ]

The values for `value` are:

* `1` - pressed
* `0` - released

See the [linux documentation for the key codes](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h).

For example, to send the <kbd>Ctrl + PageUp</kbd> combination, send:

    [{"code": 29, "value": 1}, {"code": 104, "value": 1}, {"code": 104, "value": 0}, {"code": 29, "value": 0}]

See also the example scripts `ctrl-page-up` and `ctrl-page-down`.

# Building

The C++ source can be built with [`scooter`](https://github.com/ellbur/scooter), which may be installed [from PyPi](https://pypi.org/project/scooter/) with `pip`.


Once `scooter` is installed, run:

    ./build.py

