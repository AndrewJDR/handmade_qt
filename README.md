This is a Qt platform layer for [Handmade Hero](http://handmadehero.org)

This was done as an exercise, and I don't particularly recommend Qt for this
purpose -- Something like SDL is more suitable.

That said, graphics and keyboard input are currently working reasonably well.

Qt doesn't support Gamepads, so I have no plans to add it.
It does seem to have some low-level sound support, so I'll probably look
at doing that at some point soon.

To try this, you'll have to extract your HMH sourcecode into the handmade
subdirectory and make sure it's built. You'll also need Qt. I used Qt 5.4.0.
You can open the .pro file in Qt Creator and build the project.
If you've placed your HMH source code in the correct subdirectory,
it should build fine. I used day 25/26 sources.

Also -- to run the build, you may need to copy your handmade.dll into the same
directory as the handmade_qt.exe
