/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  2 Jul 2011 12:03:01pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_ABOUTPAGE_ABOUTPAGE_B72A047D__
#define __JUCER_HEADER_ABOUTPAGE_ABOUTPAGE_B72A047D__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class AboutPage  : public Component
{
public:
    //==============================================================================
    AboutPage (const String& ip);
    ~AboutPage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The current computer's IP address.
	String ipAddress;

    //[/UserVariables]

    //==============================================================================
    Label* titleLabel;
    Label* descriptionLabel;
    Label* creditsLabel;
    Label* authorLabel;
    HyperlinkButton* niallmoodyLink;
    HyperlinkButton* juceLink;
    Label* versionLabel;
    Label* juceVersionLabel;
    Label* ipAddressLabel;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AboutPage (const AboutPage&);
    const AboutPage& operator= (const AboutPage&);
};


#endif   // __JUCER_HEADER_ABOUTPAGE_ABOUTPAGE_B72A047D__
