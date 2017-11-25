stmm-input-gtk-dm                                                  {#mainpage}
=================

Implementation of the stmm-input library for Gtkmm and Linux.

The device manager provided by this library sends events defined in the
stmm-input-ev library.

The events are generated for the following devices:
- master keyboard and pointer
- joysticks (use jstest-gtk to calibrate them)
- any device supported by plugin device managers

The device manager attaches itself to the Gtk event loop, so that its
events can easily be processed within a gtkmm application, while allowing
gtk's normal events to be received too.

Usage example:

    // printkey.cc
    #include <memory>
    #include <iostream>
    #include <cassert>
    #include <gdkmm.h>
    #include <gtkmm.h>
    #include <stmm-input-gtk-dm/stmm-input-gtk-dm.h>
    //
    int main(int argc, char** argv)
    {
        Glib::RefPtr<Gtk::Application> refApp
                    = Gtk::Application::create(argc, argv, "net.example.app");
        // device manager has to be created after gtk is initialized!
        std::shared_ptr<stmi::DeviceManager> refDM
                                        = stmi::GtkDeviceManager::create();
        if (!refDM) {
            std::cout << "Error: Couldn't create device manager" << std::endl;
            return EXIT_FAILURE;
        }
        auto refWindow = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
        // wrap the window with an accessor
        auto refAccessor = std::make_shared<stmi::GtkAccessor>(refWindow);
        // tell the device manager to track it
        refDM->addAccessor(refAccessor);
        // create a key listener
        auto refKeyListener = std::make_shared<stmi::EventListener>(
        [](const std::shared_ptr<stmi::Event>& refEvent)
        {
            assert(std::dynamic_pointer_cast<stmi::KeyEvent>(refEvent));
            auto refKeyEvent = std::static_pointer_cast<stmi::KeyEvent>(refEvent);
            std::cout << (int32_t)refKeyEvent->getKey() << std::endl;
        });
        // create a filter for listening to key events
        auto refCallIf = std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass());
        // add to the device manager
        refDM->addEventListener(refKeyListener, refCallIf);
        //
        return refApp->run(*(refWindow.operator->()));
    }

From the terminal:

    g++ printkey.cc -std=c++14 -o printkey `pkg-config --cflags --libs stmm-input-gtk-dm`
    ./printkey

Further examples can be found in the examples/ subfolder.


Warning
-------
The API of this library isn't stable yet.
