stmm-input-fake                                                    {#mainpage}
===============

Header-only library for developers that need to simulate a device manager
to feed their test code with fake events defined by the library
libstmm-input-ev.

It provides a fake device manager to which test code can add and remove 
listeners. To generate events that will be sent to the listeners it
exposes methods that simulate 
- the addition and removal of fake devices (keyboards, joysticks, etc.)
- keystrokes, mouse button clicks, joystick axis changes, etc.

Usage
-----

In your test cases instead of instantiating a device manager with 
stmi::GtkDeviceManager::create() use std::make_shared<stmi::testing::FakeDeviceManager>()
and pass it to the to be tested classes instead.

See the 'examples/' subfolder.

Warning
-------
The API of this library isn't stable yet.
