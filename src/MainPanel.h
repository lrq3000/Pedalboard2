/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  26 Nov 2011 3:32:09pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_MAINPANEL_MAINPANEL_89D8C0B__
#define __JUCER_HEADER_MAINPANEL_MAINPANEL_89D8C0B__

//[Headers]     -- You can add your own extra header files here --
#include "FilterGraph.h"
#include "ColourScheme.h"
#include "NiallsSocketLib/UDPSocket.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginField.h"
#include "MidiAppFifo.h"

class PluginListWindow;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainPanel  : public Component,
                   public MenuBarModel,
                   public ApplicationCommandTarget,
                   public MultiTimer,
                   public ChangeListener,
                   public FileBasedDocument,
                   public Thread,
                   public FileDragAndDropTarget,
                   public TextEditor::Listener,
                   public ButtonListener,
                   public ComboBoxListener,
                   public SliderListener
{
public:
    //==============================================================================
    MainPanel (ApplicationCommandManager *appManager);
    ~MainPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	For the menu bar.
	StringArray getMenuBarNames();
	///	For the menu bar.
	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName);
	///	For the menu bar.
	void menuItemSelected(int menuItemID, int topLevelMenuIndex);

	///	For keyboard shortcuts etc.
	ApplicationCommandTarget *getNextCommandTarget();
	///	For keyboard shortcuts etc.
	void getAllCommands(Array<CommandID>& commands);
	///	For keyboard shortcuts etc.
	void getCommandInfo(CommandID commandID,
						ApplicationCommandInfo& result);
	///	For keyboard shortcuts etc.
	bool perform(const InvocationInfo& info);

	///	So we know what the commandManager is.
	void setCommandManager(ApplicationCommandManager *manager);

	///	Used to add an ApplicationCommand to the queue so it'll get called in the message thread.
	void invokeCommandFromOtherThread(CommandID commandID);
	///	Used to update the tempo from a non-message thread.
	void updateTempoFromOtherThread(double tempo);

	///	Used to update the CPU usage slider.
	void timerCallback(int timerId);
	///	Used to save the plugin list.
	void changeListenerCallback(ChangeBroadcaster *changedObject);
	///	Used to update the tempo.
	void textEditorTextChanged(TextEditor &editor);
	///	Used to update the tempo.
	void textEditorReturnKeyPressed(TextEditor &editor);

	///	Used to accept dragged .pdl files.
	bool isInterestedInFileDrag(const StringArray& files);
	///	Used to accept dragged .pdl files.
	void filesDropped(const StringArray& files, int x, int y);

	///	Used to update the socket's port.
	void setSocketPort(const String& port);
	///	Used to update the socket's multicast address input.
	void setSocketMulticast(const String& address);

	///	Enables/disables the audio input.
	void enableAudioInput(bool val);
	///	Enables/disables the MIDI input.
	void enableMidiInput(bool val);
	///	Enables/disables the OSC input.
	void enableOscInput(bool val);
	///	Sets whether to automatically open the mappings window or not.
	void setAutoMappingsWindow(bool val);

	///	Where the app listens for OSC messages.
	void run();

	///	Returns the title of the current set of patches.
	String getDocumentTitle();
	///	Loads a set of patches from the passed-in file.
	Result loadDocument (const File& file);
	///	Tries to save the current patches to the passed-in file.
	Result saveDocument (const File& file);
	///	Returns the last set of patches opened.
	File getLastDocumentOpened();
	///	Saves the last set of patches opened.
	void setLastDocumentOpened (const File& file);

	///	Used to update patches from PatchOrganiser.
	ComboBox *getPatchComboBox() {return patchComboBox;};

	///	Used to clear the listWindow variable.
	void setListWindow(PluginListWindow *win) {listWindow = win;};

	///	Adds a patch from its XmlElement.
	void addPatch(XmlElement *patch);
	///	Saves the current patch to our patches array.
	void savePatch();
	///	Duplicates the indexed patch.
	void duplicatePatch(int index);
	///	Called from PatchOrganiser when it's moving patches up/down.
	void nextSwitchDoNotSavePrev();
	///	Triggers a patch change from a Midi Program Change message.
	void switchPatchFromProgramChange(int newPatch);
	///	Returns the index of the current patch.
	int getCurrentPatch() const {return patchComboBox->getSelectedId()-1;};

	///	Returns the PluginField's MidiMappingManager.
	MidiMappingManager *getMidiMappingManager() {return dynamic_cast<PluginField *>(viewport->getViewedComponent())->getMidiManager();};
	///	Returns the PluginField's AppMappingManager.
	OscMappingManager *getOscMappingManager() {return dynamic_cast<PluginField *>(viewport->getViewedComponent())->getOscManager();};

	///	Temp.
	//void logMessage(const String &message);

	///	Constants for the various menu options.
	/*!
		NOTE: Add new constants TO THE END OF THE LIST, or the user's saved
		mappings will get screwed up!
	 */
	enum
	{
		FileNew = 1,
		FileOpen,
		FileSave,
		FileSaveAs,
		FileSaveAsDefault,
		FileResetDefault,
		FileExit,
		EditDeleteConnection,
		EditOrganisePatches,
		OptionsAudio,
		OptionsPluginList,
		OptionsPreferences,
		OptionsColourSchemes,
		OptionsKeyMappings,
		HelpAbout,
		PatchNextPatch,
		PatchPrevPatch,
		TransportPlay,
		TransportRtz,
		TransportTapTempo,
		EditUserPresetManagement,
		HelpDocumentation,
		HelpLog
	};

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method. Switches patches.
	/*!
		\param newPatch Index of the new patch to load.
		\param savePrev Saves the current patch in the process.
		\param reloadPatch Reloads the current patch.
	 */
	void switchPatch(int newPatch, bool savePrev = true, bool reloadPatch = false);

	///	Helper method to load an SVG file from a binary chunk of data.
	Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom, size_t sizeInBytes);

	///	The IDs of the three timers.
	enum
	{
		CpuTimer = 0,
		MidiAppTimer,
		ProgramChangeTimer
	};

	///	The last opened document.
	static File lastDocument;

	///	Our copy of the commandManager.
	ApplicationCommandManager *commandManager;
	///	Used to display tooltips.
	TooltipWindow tooltips;

	///	The sound card settings.
	AudioDeviceManager deviceManager;
	///	The graph representing the audio signal path.
	FilterGraph signalPath;
	///	Object used to 'play' the signalPath object.
	AudioProcessorPlayer graphPlayer;
	///	The list of plugins the user can load.
	KnownPluginList pluginList;
	///	The socket we listen for OSC messages on.
	UDPSocket sock;

	///	Used to protect sock when we change port/multicast address.
	CriticalSection sockCritSec;

	///	Window to display/edit the list of possible plugins.
	PluginListWindow *listWindow;

	///	The currently-loaded patches.
	Array<XmlElement *> patches;
	///	The index of the current patch.
	int currentPatch;
	///	Used to switch patches via Midi Program Changes.
	int programChangePatch;

	///	The two drawables we use for the playButton.
	ScopedPointer<Drawable> playImage;
	ScopedPointer<Drawable> pauseImage;
	///	Whether the playPauseButton is currently displaying the play icon.
	bool playing;

	///	Used to compensate for the stupid way the combo box handles the user editing its text.
	int lastCombo;

	///	Used to ensure patches don't get overwritten when they're being re-ordered in PatchOrganiser.
	bool doNotSaveNextPatch;

	///	Used for tap tempo when the user does it via keyboard.
	int64 lastTempoTicks;

	///	Used to pass messages from the audio thread to the message thread.
	MidiAppFifo midiAppFifo;

	///	Simple Component to pass to warningBox.
	class ProgramChangeWarning : public Component
	{
	  public:
		///	Constructor.
		ProgramChangeWarning():
		index(0)
		{
			setSize(250, 150);
		};
		///	Destructor.
		~ProgramChangeWarning() {};

		///	Draws the warning.
		void paint(Graphics& g)
		{
			String tempstr;
			Font smallFont(24.0f);
			Font bigFont(48.0f, Font::bold);
	
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);

			g.setFont(smallFont);
			g.drawText(L"Out of bounds",
					   0,
					   0,
					   250,
					   50,
					   Justification(Justification::centred),
					   false);
			g.drawText(L"MIDI Program Change",
					   0,
					   24,
					   250,
					   50,
					   Justification(Justification::centred),
					   false);
			g.drawText(L"received:",
					   0,
					   48,
					   250,
					   50,
					   Justification(Justification::centred),
					   false);

			tempstr << index;
			g.setFont(bigFont);
			g.drawText(tempstr,
					   0,
					   88,
					   250,
					   50,
					   Justification(Justification::centred),
					   false);
		};
		///	Sets the offending index.
		void setIndex(int i)
		{
			index = i;
		};
	  private:
		///	The offending index.
		int index;
	};
	///	Used to inform the user when the user does a MIDI program change outside the limits of the patch list.
	ProgramChangeWarning warningText;
	///	Used to inform the user when the user does a MIDI program change outside the limits of the patch list.
	ScopedPointer<CallOutBox> warningBox;

	///	Temp.
	//FileOutputStream outFile;

    //[/UserVariables]

    //==============================================================================
    Label* patchLabel;
    TextButton* prevPatch;
    TextButton* nextPatch;
    ComboBox* patchComboBox;
    Viewport* viewport;
    Slider* cpuSlider;
    Label* cpuLabel;
    DrawableButton* playButton;
    DrawableButton* rtzButton;
    Label* tempoLabel;
    TextEditor* tempoEditor;
    ArrowButton* tapTempoButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MainPanel (const MainPanel&);
    const MainPanel& operator= (const MainPanel&);
};


#endif   // __JUCER_HEADER_MAINPANEL_MAINPANEL_89D8C0B__
