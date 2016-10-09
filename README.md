stmm-input
==========

Extensible, platform independent device input event library for C++ (C++14).

This source package contains:

- libstmm-input:
    library that defines the platform-independent core interfaces for
    events, devices, capabilities, device managers and event listeners.

- libstmm-input-ev:
    library that defines a set of event types suitable for games.

- libstmm-input-base:
    library of helper classes to ease the implementation and composition of
    device managers.

- libstmm-input-gtk:
    library that implements a device manager that integrates into Gtk's
    main event loop sending events defined by libstmm-input-ev to listeners.
    It supports the following devices:
    + master keyboard and pointer.
    + floating keyboards (readied with device-floater)
    + joysticks (calibrated with jstest-gtk, jscal, jscal-store)

- libstmm-input-fake:
    header-only library for developers that need to simulate a device manager
    to feed their test code with events defined by libstmm-input-ev.

- device-floater:
    simple gui application to safely float keyboard and pointer devices
    managed by X11 (XI2). It provides a subset of the 'xinput float' command.

Read the INSTALL file for installation instructions.

Examples can be found in the libstmm-input-gtk subfolder.


Warning
-------
The APIs of the libraries aren't stable yet.
