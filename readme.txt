Pedalboard2
-----------

Description
-----------
A VST plugin host designed for live use. Based on the JUCE 'audio plugin host'
code, heavily modified to better suit my purposes.

Key Features:
- Modular patching.
- All plugin parameters can be manipulated in real time via MIDI CCs or Open
  Sound Control messages.
- Any number of patches can be queued up and rapidly switched.


Usage
-----
Should be fairly self-explanatory. Double click in the plugin field to add
processors. To add VST plugins to the available list of processors, go to
Options->Plugin List, the Options...->Scan for new or updated VST plugins...
All keyboard shortcuts can be remapped via Options->Application Mappings; you
can also have MIDI CCs or OSC messages mapped to the commands here (useful for
the next/prev patch commands).

To delete connections, click to select, then hit the delete key.

If you don't want the app to listen for OSC messages, untoggle the OSC Input
button in Options->Misc Settings.

The big play button at the bottom of the app is just so you can sync multiple
File Players up to a single trigger - for now all it does is play and pause any
connected File Players.

For more details, see the html documentation.


NOTE: If you're running a 64-bit version of Windows *AND* you're using 64-bit
plugins, you'll want to use the Pedalboard2-64bit.exe version. If you want to
use 32-bit plugins, go with Pedalboard2.exe


Files
-----
Settings files are stored:

Documents and Settings/<user name>/Application Data/Pedalboard2 (Windows XP)
Users/<user name>/AppData/Roaming/Pedalboard2 (Windows Vista/7)
/Users/<user name>/Library/Application Support/Pedalboard2 (OSX)


License Details
---------------
Source code is licensed under the GPL v3.  Graphics and sounds under the
Creative Commons Sampling Plus license.


- Niall Moody (22/04/13).
