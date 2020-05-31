stmm-input-doc                                                       {#mainpage}
==============

Extensible device input event libraries for C++ documentation.

The main libraries are:

- libstmm-input:
    library that defines the platform-independent core interfaces for
    events, devices, capabilities, device managers and event listeners.

- libstmm-input-base:
    library of helper classes to ease the implementation and composition of
    device managers.

- libstmm-input-dl:
    library that implemens a plugin device manager which loads other device managers
    at run-time as children.

- libstmm-input-ev:
    library that defines a set of event types suitable for games.

- libstmm-input-gtk:
    library that defines a Gtk::Window accessor for device managers.

- libstmm-input-gtk-dm:
    library that implements a device manager that integrates into Gtk's
    main event loop sending events defined by libstmm-input-ev to listeners.
    It supports the following devices:
    + master keyboard and pointer.
    + joysticks (calibrated with jstest-gtk, jscal, jscal-store)
    + any device supported by plug in device managers

- libstmm-input-fake:
    library meant for developers that need to simulate a device manager
    to feed their test code with events defined by libstmm-input-ev.


Examples can be found in the libstmm-input-gtk-dm and libstmm-input-fake subfolders.


Warning
-------
The APIs of the libraries aren't stable yet.
