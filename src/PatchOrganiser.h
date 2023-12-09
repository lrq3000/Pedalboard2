/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  5 Jun 2011 2:06:45pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__
#define __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class MainPanel;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PatchOrganiser  : public Component,
                        public ListBoxModel,
                        public Label::Listener,
                        public ButtonListener
{
public:
    //==============================================================================
    PatchOrganiser (MainPanel *panel, Array<XmlElement *>& patchArray);
    ~PatchOrganiser();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Returns the number of active mappings.
	int getNumRows();
	///	Draws a row.
	void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected);
	///	Returns the component for a single row.
	Component *refreshComponentForRow(int rowNumber, bool isRowSelected, Component *existingComponentToUpdate);
	///	So the user can select rows.
	void listBoxItemClicked(int row, const MouseEvent &e);
	///	So the user can deselect rows.
	void backgroundClicked();

	///	Called when the user changes a patch name by editing the label.
	void labelTextChanged(Label *labelThatHasChanged);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The MainPanel.
	MainPanel *mainPanel;
	///	Our copy of the available patches.
	Array<XmlElement *>& patches;

    //[/UserVariables]

    //==============================================================================
    ListBox* patchList;
    TextButton* addButton;
    TextButton* copyButton;
    TextButton* removeButton;
    TextButton* moveUpButton;
    TextButton* moveDownButton;
    TextButton* importButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    PatchOrganiser (const PatchOrganiser&);
    const PatchOrganiser& operator= (const PatchOrganiser&);
};


#endif   // __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__
