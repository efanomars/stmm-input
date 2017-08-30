stmm-input-gtk                                                     {#mainpage}
==============

Common Gtkmm window accessor for the stmm-input library.

The accessor defined by this library allows gtkmm specific device managers
to implement a common behavior; conveying events from devices normally
unsupported by it (like joysticks) to the currently focused window.

This means that if for example a joystick button is pressed when a
certain window has focus and the window loses focus or is closed before
the button is released, the listeners will receive a cancel event for that
button and the eventual release event will not be sent for the newly
focused window.




Warning
-------
The API of this library isn't stable yet.
