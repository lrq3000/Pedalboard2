//	TrayIcon.h - System tray icon.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2011 Niall Moody.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#ifndef TRAYICON_H_
#define TRAYICON_H_

#ifndef __APPLE__

#include "../JuceLibraryCode/JuceHeader.h"

///	System tray icon.
class TrayIcon : public SystemTrayIconComponent
{
  public:
	///	Constructor.
	TrayIcon(DocumentWindow *win);
	///	Destructor.
	~TrayIcon();

	///	Called to display a PopupMenu on right-click.
	void mouseDown(const MouseEvent& e);
	///	Called to display/hid the main window.
	void mouseDoubleClick(const MouseEvent& e);
  private:
	///	Our copy of the main window.
	DocumentWindow *window;
};

#endif

#endif
