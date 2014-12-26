handmade_qt
============

This is a Qt platform layer for [Handmade Hero](http://handmadehero.org)

This was done as an exercise, and I don't particularly recommend Qt for this
purpose -- Something like SDL is more suitable for performance.

Progress
-------------
What's in:
* Graphics
* Keyboard input
* Baisc performance counters
* Game memory alloc (Very simple though -- uses new[])

I'd prefer to keep this a pure Qt layer without any other deps or platform specific code, so there's some things I probably won't do:
* Gamepads (No gamepad support in Qt)
* Hot reloading/replay (No LoadLibrary/etc abstraction in Qt)

For the future:
* I want to make use of Qt's hardware acceleration features so I can see how much of a difference it makes. For the time being, I went out of my way to do things in naive, software based way with QImage/QPainter to see just how bad the performance is.
* Qt does seem to have some [low-level sound support](http://doc.qt.io/qt-5/qaudiooutput.html), so I'll probably look
at doing that at some point soon.
* Qt is on a ton of platforms, including some obscure embeded ones, so that could also be interesting to try out down the road.

Building
-------------
To build this, you'll have to extract your HMH sourcecode into the handmade
subdirectory and make sure it's built. You'll also need Qt -- I used Qt 5.4.0
but could probably get away with an older version.
You can open the .pro file in Qt Creator and build the project.
Assuming you've placed your built HMH source code in the correct subdirectory,
it should compile fine. I used the day 29 sources.

To run the build, you may need to copy your handmade.dll into the same
directory as the handmade_qt.exe

I've only tested this on Windows, but I'm not using any exotic Qt features, so 
it might just work on any of the [platforms Qt runs on](http://doc.qt.io/qt-5/supported-platforms.html)
