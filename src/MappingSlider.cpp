//	MappingSlider.cpp - A two-tick Slider which can handle inverted values.
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

#include "MappingSlider.h"

class MappingSlider::PopupDisplayComponent  : public BubbleComponent,
                                       public Timer
{
public:
    //==============================================================================
    PopupDisplayComponent (MappingSlider& owner_)
        : owner (owner_),
          font (15.0f, Font::bold)
    {
        setAlwaysOnTop (true);
    }

    void paintContent (Graphics& g, int w, int h)
    {
        g.setFont (font);
        g.setColour (findColour (TooltipWindow::textColourId, true));
        g.drawFittedText (text, 0, 0, w, h, Justification::centred, 1);
    }

    void getContentSize (int& w, int& h)
    {
        w = font.getStringWidth (text) + 18;
        h = (int) (font.getHeight() * 1.6f);
    }

    void updatePosition (const String& newText)
    {
        if (text != newText)
        {
            text = newText;
            repaint();
        }

        BubbleComponent::setPosition (&owner);
        repaint();
    }

    void timerCallback()
    {
        owner.popupDisplay = nullptr;
    }

private:
    MappingSlider& owner;
    Font font;
    String text;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupDisplayComponent);
};

//==============================================================================
MappingSlider::MappingSlider (const String& name)
  : Component (name),
    lastCurrentValue (0),
    lastValueMin (0),
    lastValueMax (0),
    minimum (0),
    maximum (10),
    interval (0),
    skewFactor (1.0),
    velocityModeSensitivity (1.0),
    velocityModeOffset (0.0),
    velocityModeThreshold (1),
    rotaryStart (float_Pi * 1.2f),
    rotaryEnd (float_Pi * 2.8f),
    numDecimalPlaces (7),
    sliderRegionStart (0),
    sliderRegionSize (1),
    sliderBeingDragged (-1),
    pixelsForFullDragExtent (250),
    style (TwoValueHorizontal),
    textBoxPos (TextBoxLeft),
    textBoxWidth (80),
    textBoxHeight (20),
    incDecButtonMode (incDecButtonsNotDraggable),
    editableText (true),
    doubleClickToValue (false),
    isVelocityBased (false),
    userKeyOverridesVelocity (true),
    rotaryStop (true),
    incDecButtonsSideBySide (false),
    sendChangeOnlyOnRelease (false),
    popupDisplayEnabled (false),
    menuEnabled (false),
    menuShown (false),
    scrollWheelEnabled (true),
    snapsToMousePos (true),
    parentForPopupDisplay (nullptr)
{
    setWantsKeyboardFocus (false);
    setRepaintsOnMouseActivity (true);

    MappingSlider::lookAndFeelChanged();
    updateText();

    currentValue.addListener (this);
    valueMin.addListener (this);
    valueMax.addListener (this);
}

MappingSlider::~MappingSlider()
{
    currentValue.removeListener (this);
    valueMin.removeListener (this);
    valueMax.removeListener (this);
    popupDisplay = nullptr;
}


//==============================================================================
void MappingSlider::handleAsyncUpdate()
{
    cancelPendingUpdate();

    Component::BailOutChecker checker (this);
    listeners.callChecked (checker, &MappingSliderListener::sliderValueChanged, this);  // (can't use MappingSlider::Listener due to idiotic VC2005 bug)
}

void MappingSlider::sendDragStart()
{
    startedDragging();

    Component::BailOutChecker checker (this);
    listeners.callChecked (checker, &MappingSliderListener::sliderDragStarted, this);
}

void MappingSlider::sendDragEnd()
{
    stoppedDragging();

    sliderBeingDragged = -1;

    Component::BailOutChecker checker (this);
    listeners.callChecked (checker, &MappingSliderListener::sliderDragEnded, this);
}

void MappingSlider::addListener (MappingSliderListener* const listener)
{
    listeners.add (listener);
}

void MappingSlider::removeListener (MappingSliderListener* const listener)
{
    listeners.remove (listener);
}

//==============================================================================
void MappingSlider::setMappingSliderStyle (const MappingSliderStyle newStyle)
{
    if (style != newStyle)
    {
        style = newStyle;
        repaint();
        lookAndFeelChanged();
    }
}

void MappingSlider::setRotaryParameters (const float startAngleRadians,
                                  const float endAngleRadians,
                                  const bool stopAtEnd)
{
    // make sure the values are sensible..
    jassert (rotaryStart >= 0 && rotaryEnd >= 0);
    jassert (rotaryStart < float_Pi * 4.0f && rotaryEnd < float_Pi * 4.0f);
    jassert (rotaryStart < rotaryEnd);

    rotaryStart = startAngleRadians;
    rotaryEnd = endAngleRadians;
    rotaryStop = stopAtEnd;
}

void MappingSlider::setVelocityBasedMode (const bool velBased)
{
    isVelocityBased = velBased;
}

void MappingSlider::setVelocityModeParameters (const double sensitivity,
                                        const int threshold,
                                        const double offset,
                                        const bool userCanPressKeyToSwapMode)
{
    jassert (threshold >= 0);
    jassert (sensitivity > 0);
    jassert (offset >= 0);

    velocityModeSensitivity = sensitivity;
    velocityModeOffset = offset;
    velocityModeThreshold = threshold;
    userKeyOverridesVelocity = userCanPressKeyToSwapMode;
}

void MappingSlider::setSkewFactor (const double factor)
{
    skewFactor = factor;
}

void MappingSlider::setSkewFactorFromMidPoint (const double sliderValueToShowAtMidPoint)
{
    if (maximum > minimum)
        skewFactor = log (0.5) / log ((sliderValueToShowAtMidPoint - minimum)
                                        / (maximum - minimum));
}

void MappingSlider::setMouseDragSensitivity (const int distanceForFullScaleDrag)
{
    jassert (distanceForFullScaleDrag > 0);

    pixelsForFullDragExtent = distanceForFullScaleDrag;
}

void MappingSlider::setIncDecButtonsMode (const IncDecButtonMode mode)
{
    if (incDecButtonMode != mode)
    {
        incDecButtonMode = mode;
        lookAndFeelChanged();
    }
}

void MappingSlider::setTextBoxStyle (const TextEntryBoxPosition newPosition,
                              const bool isReadOnly,
                              const int textEntryBoxWidth,
                              const int textEntryBoxHeight)
{
    if (textBoxPos != newPosition
         || editableText != (! isReadOnly)
         || textBoxWidth != textEntryBoxWidth
         || textBoxHeight != textEntryBoxHeight)
    {
        textBoxPos = newPosition;
        editableText = ! isReadOnly;
        textBoxWidth = textEntryBoxWidth;
        textBoxHeight = textEntryBoxHeight;

        repaint();
        lookAndFeelChanged();
    }
}

void MappingSlider::setTextBoxIsEditable (const bool shouldBeEditable)
{
    editableText = shouldBeEditable;

    if (valueBox != nullptr)
        valueBox->setEditable (shouldBeEditable && isEnabled());
}

void MappingSlider::showTextBox()
{
    jassert (editableText); // this should probably be avoided in read-only sliders.

    if (valueBox != nullptr)
        valueBox->showEditor();
}

void MappingSlider::hideTextBox (const bool discardCurrentEditorContents)
{
    if (valueBox != nullptr)
    {
        valueBox->hideEditor (discardCurrentEditorContents);

        if (discardCurrentEditorContents)
            updateText();
    }
}

void MappingSlider::setChangeNotificationOnlyOnRelease (const bool onlyNotifyOnRelease)
{
    sendChangeOnlyOnRelease = onlyNotifyOnRelease;
}

void MappingSlider::setMappingSliderSnapsToMousePosition (const bool shouldSnapToMouse)
{
    snapsToMousePos = shouldSnapToMouse;
}

void MappingSlider::setPopupDisplayEnabled (const bool enabled,
                                     Component* const parentComponentToUse)
{
    popupDisplayEnabled = enabled;
    parentForPopupDisplay = parentComponentToUse;
}

//==============================================================================
void MappingSlider::colourChanged()
{
    lookAndFeelChanged();
}

void MappingSlider::lookAndFeelChanged()
{
    //LookAndFeel& lf = getLookAndFeel();

    if (textBoxPos != NoTextBox)
    {
        const String previousTextBoxContent (valueBox != nullptr ? valueBox->getText()
                                                                 : getTextFromValue (currentValue.getValue()));

        valueBox = nullptr;
        addAndMakeVisible (valueBox = createMappingSliderTextBox (*this));

        valueBox->setWantsKeyboardFocus (false);
        valueBox->setText (previousTextBoxContent, dontSendNotification);

        if (valueBox->isEditable() != editableText) // (avoid overriding the single/double click flags unless we have to)
            valueBox->setEditable (editableText && isEnabled());

        valueBox->addListener (this);

        if (style == LinearBar)
            valueBox->addMouseListener (this, false);

        valueBox->setTooltip (getTooltip());
    }
    else
    {
        valueBox = nullptr;
    }

    if (style == IncDecButtons)
    {
        addAndMakeVisible (incButton = createMappingSliderButton (true));
        incButton->addListener (this);

        addAndMakeVisible (decButton = createMappingSliderButton (false));
        decButton->addListener (this);

        if (incDecButtonMode != incDecButtonsNotDraggable)
        {
            incButton->addMouseListener (this, false);
            decButton->addMouseListener (this, false);
        }
        else
        {
            incButton->setRepeatSpeed (300, 100, 20);
            incButton->addMouseListener (decButton, false);

            decButton->setRepeatSpeed (300, 100, 20);
            decButton->addMouseListener (incButton, false);
        }

        incButton->setTooltip (getTooltip());
        decButton->setTooltip (getTooltip());
    }
    else
    {
        incButton = nullptr;
        decButton = nullptr;
    }

    setComponentEffect (getMappingSliderEffect());

    resized();
    repaint();
}

//==============================================================================
void MappingSlider::setRange (const double newMin,
                       const double newMax,
                       const double newInt)
{
    if (minimum != newMin
        || maximum != newMax
        || interval != newInt)
    {
        minimum = newMin;
        maximum = newMax;
        interval = newInt;

        // figure out the number of DPs needed to display all values at this
        // interval setting.
        numDecimalPlaces = 7;

        if (newInt != 0)
        {
            int v = abs ((int) (newInt * 10000000));

            while ((v % 10) == 0)
            {
                --numDecimalPlaces;
                v /= 10;
            }
        }

        // keep the current values inside the new range..
        if (style != TwoValueHorizontal && style != TwoValueVertical)
        {
            setValue (getValue(), false, false);
        }
        else
        {
            setMinValue (getMinValue(), false, false);
            setMaxValue (getMaxValue(), false, false);
        }

        updateText();
    }
}

void MappingSlider::triggerChangeMessage (const bool synchronous)
{
    if (synchronous)
        handleAsyncUpdate();
    else
        triggerAsyncUpdate();

    valueChanged();
}

void MappingSlider::valueChanged (Value& value)
{
    if (value.refersToSameSourceAs (currentValue))
    {
        if (style != TwoValueHorizontal && style != TwoValueVertical)
            setValue (currentValue.getValue(), false, false);
    }
    else if (value.refersToSameSourceAs (valueMin))
        setMinValue (valueMin.getValue(), false, false, true);
    else if (value.refersToSameSourceAs (valueMax))
        setMaxValue (valueMax.getValue(), false, false, true);
}

double MappingSlider::getValue() const
{
    // for a two-value style slider, you should use the getMinValue() and getMaxValue()
    // methods to get the two values.
    jassert (style != TwoValueHorizontal && style != TwoValueVertical);

    return currentValue.getValue();
}

void MappingSlider::setValue (double newValue,
                       const bool sendUpdateMessage,
                       const bool sendMessageSynchronously)
{
    // for a two-value style slider, you should use the setMinValue() and setMaxValue()
    // methods to set the two values.
    jassert (style != TwoValueHorizontal && style != TwoValueVertical);

    newValue = constrainedValue (newValue);

    if (style == ThreeValueHorizontal || style == ThreeValueVertical)
    {
        jassert ((double) valueMin.getValue() <= (double) valueMax.getValue());

        newValue = jlimit ((double) valueMin.getValue(),
                           (double) valueMax.getValue(),
                           newValue);
    }

    if (newValue != lastCurrentValue)
    {
        if (valueBox != nullptr)
            valueBox->hideEditor (true);

        lastCurrentValue = newValue;

        // (need to do this comparison because the Value will use equalsWithSameType to compare
        // the new and old values, so will generate unwanted change events if the type changes)
        if (currentValue != newValue)
            currentValue = newValue;

        updateText();
        repaint();

        if (popupDisplay != nullptr)
            popupDisplay->updatePosition (getTextFromValue (newValue));

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

double MappingSlider::getMinValue() const
{
    // The minimum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    return valueMin.getValue();
}

double MappingSlider::getMaxValue() const
{
    // The maximum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    return valueMax.getValue();
}

void MappingSlider::setMinValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously, const bool allowNudgingOfOtherValues)
{
    // The minimum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    newValue = constrainedValue (newValue);

    /*if (style == TwoValueHorizontal || style == TwoValueVertical)
    {
        if (allowNudgingOfOtherValues && newValue > (double) valueMax.getValue())
            setMaxValue (newValue, sendUpdateMessage, sendMessageSynchronously);

        newValue = jmin ((double) valueMax.getValue(), newValue);
    }
    else
    {
        if (allowNudgingOfOtherValues && newValue > lastCurrentValue)
            setValue (newValue, sendUpdateMessage, sendMessageSynchronously);

        newValue = jmin (lastCurrentValue, newValue);
    }*/

    if (lastValueMin != newValue)
    {
        lastValueMin = newValue;
        valueMin = newValue;
        repaint();

        if (popupDisplay != nullptr)
            popupDisplay->updatePosition (getTextFromValue (newValue));

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void MappingSlider::setMaxValue (double newValue, const bool sendUpdateMessage, const bool sendMessageSynchronously, const bool allowNudgingOfOtherValues)
{
    // The maximum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    newValue = constrainedValue (newValue);

    /*if (style == TwoValueHorizontal || style == TwoValueVertical)
    {
        if (allowNudgingOfOtherValues && newValue < (double) valueMin.getValue())
            setMinValue (newValue, sendUpdateMessage, sendMessageSynchronously);

        newValue = jmax ((double) valueMin.getValue(), newValue);
    }
    else
    {
        if (allowNudgingOfOtherValues && newValue < lastCurrentValue)
            setValue (newValue, sendUpdateMessage, sendMessageSynchronously);

        newValue = jmax (lastCurrentValue, newValue);
    }*/

    if (lastValueMax != newValue)
    {
        lastValueMax = newValue;
        valueMax = newValue;
        repaint();

        if (popupDisplay != nullptr)
            popupDisplay->updatePosition (getTextFromValue (valueMax.getValue()));

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void MappingSlider::setMinAndMaxValues (double newMinValue, double newMaxValue, bool sendUpdateMessage, bool sendMessageSynchronously)
{
    // The maximum value only applies to sliders that are in two- or three-value mode.
    jassert (style == TwoValueHorizontal || style == TwoValueVertical
              || style == ThreeValueHorizontal || style == ThreeValueVertical);

    if (newMaxValue < newMinValue)
        std::swap (newMaxValue, newMinValue);

    newMinValue = constrainedValue (newMinValue);
    newMaxValue = constrainedValue (newMaxValue);

    if (lastValueMax != newMaxValue || lastValueMin != newMinValue)
    {
        lastValueMax = newMaxValue;
        lastValueMin = newMinValue;
        valueMin = newMinValue;
        valueMax = newMaxValue;
        repaint();

        if (sendUpdateMessage)
            triggerChangeMessage (sendMessageSynchronously);
    }
}

void MappingSlider::setDoubleClickReturnValue (const bool isDoubleClickEnabled,
                                        const double valueToSetOnDoubleClick)
{
    doubleClickToValue = isDoubleClickEnabled;
    doubleClickReturnValue = valueToSetOnDoubleClick;
}

double MappingSlider::getDoubleClickReturnValue (bool& isEnabled_) const
{
    isEnabled_ = doubleClickToValue;
    return doubleClickReturnValue;
}

void MappingSlider::updateText()
{
    if (valueBox != nullptr)
        valueBox->setText (getTextFromValue (currentValue.getValue()), dontSendNotification);
}

void MappingSlider::setTextValueSuffix (const String& suffix)
{
    if (textSuffix != suffix)
    {
        textSuffix = suffix;
        updateText();
    }
}

String MappingSlider::getTextValueSuffix() const
{
    return textSuffix;
}

const String MappingSlider::getTextFromValue (double v)
{
    if (getNumDecimalPlacesToDisplay() > 0)
        return String (v, getNumDecimalPlacesToDisplay()) + getTextValueSuffix();
    else
        return String (roundToInt (v)) + getTextValueSuffix();
}

double MappingSlider::getValueFromText (const String& text)
{
    String t (text.trimStart());

    if (t.endsWith (textSuffix))
        t = t.substring (0, t.length() - textSuffix.length());

    while (t.startsWithChar ('+'))
        t = t.substring (1).trimStart();

    return t.initialSectionContainingOnly ("0123456789.,-")
            .getDoubleValue();
}

double MappingSlider::proportionOfLengthToValue (double proportion)
{
    if (skewFactor != 1.0 && proportion > 0.0)
        proportion = exp (log (proportion) / skewFactor);

    return minimum + (maximum - minimum) * proportion;
}

double MappingSlider::valueToProportionOfLength (double value)
{
    const double n = (value - minimum) / (maximum - minimum);

    return skewFactor == 1.0 ? n : pow (n, skewFactor);
}

double MappingSlider::snapValue (double attemptedValue, const bool)
{
    return attemptedValue;
}

//==============================================================================
void MappingSlider::startedDragging()
{
}

void MappingSlider::stoppedDragging()
{
}

void MappingSlider::valueChanged()
{
}

//==============================================================================
void MappingSlider::enablementChanged()
{
    repaint();
}

void MappingSlider::setPopupMenuEnabled (const bool menuEnabled_)
{
    menuEnabled = menuEnabled_;
}

void MappingSlider::setScrollWheelEnabled (const bool enabled)
{
    scrollWheelEnabled = enabled;
}

//==============================================================================
void MappingSlider::labelTextChanged (Label* label)
{
    const double newValue = snapValue (getValueFromText (label->getText()), false);

    if (newValue != (double) currentValue.getValue())
    {
        sendDragStart();
        setValue (newValue, true, true);
        sendDragEnd();
    }

    updateText(); // force a clean-up of the text, needed in case setValue() hasn't done this.
}

void MappingSlider::buttonClicked (Button* button)
{
    if (style == IncDecButtons)
    {
        sendDragStart();

        if (button == incButton)
            setValue (snapValue (getValue() + interval, false), true, true);
        else if (button == decButton)
            setValue (snapValue (getValue() - interval, false), true, true);

        sendDragEnd();
    }
}

//==============================================================================
double MappingSlider::constrainedValue (double value) const
{
    if (interval > 0)
        value = minimum + interval * std::floor ((value - minimum) / interval + 0.5);

    if (value <= minimum || maximum <= minimum)
        value = minimum;
    else if (value >= maximum)
        value = maximum;

    return value;
}

float MappingSlider::getLinearMappingSliderPos (const double value)
{
    double sliderPosProportional;

    if (maximum > minimum)
    {
        if (value < minimum)
        {
            sliderPosProportional = 0.0;
        }
        else if (value > maximum)
        {
            sliderPosProportional = 1.0;
        }
        else
        {
            sliderPosProportional = valueToProportionOfLength (value);
            jassert (sliderPosProportional >= 0 && sliderPosProportional <= 1.0);
        }
    }
    else
    {
        sliderPosProportional = 0.5;
    }

    if (isVertical() || style == IncDecButtons)
        sliderPosProportional = 1.0 - sliderPosProportional;

    return (float) (sliderRegionStart + sliderPosProportional * sliderRegionSize);
}

bool MappingSlider::isHorizontal() const
{
    return style == LinearHorizontal
        || style == LinearBar
        || style == TwoValueHorizontal
        || style == ThreeValueHorizontal;
}

bool MappingSlider::isVertical() const
{
    return style == LinearVertical
        || style == TwoValueVertical
        || style == ThreeValueVertical;
}

bool MappingSlider::incDecDragDirectionIsHorizontal() const
{
    return incDecButtonMode == incDecButtonsDraggable_Horizontal
            || (incDecButtonMode == incDecButtonsDraggable_AutoDirection && incDecButtonsSideBySide);
}

float MappingSlider::getPositionOfValue (const double value)
{
    if (isHorizontal() || isVertical())
    {
        return getLinearMappingSliderPos (value);
    }
    else
    {
        jassertfalse; // not a valid call on a slider that doesn't work linearly!
        return 0.0f;
    }
}

//==============================================================================
void MappingSlider::paint (Graphics& g)
{
    if (style != IncDecButtons)
    {
        if (style == Rotary || style == RotaryHorizontalDrag || style == RotaryVerticalDrag)
        {
            const float sliderPos = (float) valueToProportionOfLength (lastCurrentValue);
            jassert (sliderPos >= 0 && sliderPos <= 1.0f);

            drawRotaryMappingSlider (g,
                                               sliderRect.getX(),
                                               sliderRect.getY(),
                                               sliderRect.getWidth(),
                                               sliderRect.getHeight(),
                                               sliderPos,
                                               rotaryStart, rotaryEnd,
                                               *this);
        }
        else
        {
            drawLinearMappingSlider (g,
                                               sliderRect.getX(),
                                               sliderRect.getY(),
                                               sliderRect.getWidth(),
                                               sliderRect.getHeight(),
                                               getLinearMappingSliderPos (lastCurrentValue),
                                               getLinearMappingSliderPos (lastValueMin),
                                               getLinearMappingSliderPos (lastValueMax),
                                               style,
                                               *this);
        }

        if (style == LinearBar && valueBox == nullptr)
        {
            g.setColour (findColour (MappingSlider::textBoxOutlineColourId));
            g.drawRect (0, 0, getWidth(), getHeight(), 1);
        }
    }
}

void MappingSlider::resized()
{
    int minXSpace = 0;
    int minYSpace = 0;

    if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
        minXSpace = 30;
    else
        minYSpace = 15;

    const int tbw = jmax (0, jmin (textBoxWidth, getWidth() - minXSpace));
    const int tbh = jmax (0, jmin (textBoxHeight, getHeight() - minYSpace));

    if (style == LinearBar)
    {
        if (valueBox != nullptr)
            valueBox->setBounds (getLocalBounds());
    }
    else
    {
        if (textBoxPos == NoTextBox)
        {
            sliderRect = getLocalBounds();
        }
        else if (textBoxPos == TextBoxLeft)
        {
            valueBox->setBounds (0, (getHeight() - tbh) / 2, tbw, tbh);
            sliderRect.setBounds (tbw, 0, getWidth() - tbw, getHeight());
        }
        else if (textBoxPos == TextBoxRight)
        {
            valueBox->setBounds (getWidth() - tbw, (getHeight() - tbh) / 2, tbw, tbh);
            sliderRect.setBounds (0, 0, getWidth() - tbw, getHeight());
        }
        else if (textBoxPos == TextBoxAbove)
        {
            valueBox->setBounds ((getWidth() - tbw) / 2, 0, tbw, tbh);
            sliderRect.setBounds (0, tbh, getWidth(), getHeight() - tbh);
        }
        else if (textBoxPos == TextBoxBelow)
        {
            valueBox->setBounds ((getWidth() - tbw) / 2, getHeight() - tbh, tbw, tbh);
            sliderRect.setBounds (0, 0, getWidth(), getHeight() - tbh);
        }
    }

    const int indent = getMappingSliderThumbRadius (*this);

    if (style == LinearBar)
    {
        const int barIndent = 1;
        sliderRegionStart = barIndent;
        sliderRegionSize = getWidth() - barIndent * 2;

        sliderRect.setBounds (sliderRegionStart, barIndent,
                              sliderRegionSize, getHeight() - barIndent * 2);
    }
    else if (isHorizontal())
    {
        sliderRegionStart = sliderRect.getX() + indent;
        sliderRegionSize = jmax (1, sliderRect.getWidth() - indent * 2);

        sliderRect.setBounds (sliderRegionStart, sliderRect.getY(),
                              sliderRegionSize, sliderRect.getHeight());
    }
    else if (isVertical())
    {
        sliderRegionStart = sliderRect.getY() + indent;
        sliderRegionSize = jmax (1, sliderRect.getHeight() - indent * 2);

        sliderRect.setBounds (sliderRect.getX(), sliderRegionStart,
                              sliderRect.getWidth(), sliderRegionSize);
    }
    else
    {
        sliderRegionStart = 0;
        sliderRegionSize = 100;
    }

    if (style == IncDecButtons)
    {
        Rectangle<int> buttonRect (sliderRect);

        if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
            buttonRect.expand (-2, 0);
        else
            buttonRect.expand (0, -2);

        incDecButtonsSideBySide = buttonRect.getWidth() > buttonRect.getHeight();

        if (incDecButtonsSideBySide)
        {
            decButton->setBounds (buttonRect.getX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth() / 2,
                                  buttonRect.getHeight());

            decButton->setConnectedEdges (Button::ConnectedOnRight);

            incButton->setBounds (buttonRect.getCentreX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth() / 2,
                                  buttonRect.getHeight());

            incButton->setConnectedEdges (Button::ConnectedOnLeft);
        }
        else
        {
            incButton->setBounds (buttonRect.getX(),
                                  buttonRect.getY(),
                                  buttonRect.getWidth(),
                                  buttonRect.getHeight() / 2);

            incButton->setConnectedEdges (Button::ConnectedOnBottom);

            decButton->setBounds (buttonRect.getX(),
                                  buttonRect.getCentreY(),
                                  buttonRect.getWidth(),
                                  buttonRect.getHeight() / 2);

            decButton->setConnectedEdges (Button::ConnectedOnTop);
        }
    }
}

void MappingSlider::focusOfChildComponentChanged (FocusChangeType)
{
    repaint();
}

static void sliderMenuCallback (int result, MappingSlider* slider)
{
    if (slider != nullptr)
    {
        switch (result)
        {
            case 1: slider->setVelocityBasedMode (! slider->getVelocityBasedMode()); break;
            case 2: slider->setMappingSliderStyle (MappingSlider::Rotary); break;
            case 3: slider->setMappingSliderStyle (MappingSlider::RotaryHorizontalDrag); break;
            case 4: slider->setMappingSliderStyle (MappingSlider::RotaryVerticalDrag); break;
            default: break;
        }
    }
}

void MappingSlider::mouseDown (const MouseEvent& e)
{
    mouseWasHidden = false;
    incDecDragged = false;
    mousePosWhenLastDragged = e.getPosition();
    mouseDragStartX = e.getMouseDownX();
    mouseDragStartY = e.getMouseDownY();

    if (isEnabled())
    {
        if (e.mods.isPopupMenu() && menuEnabled)
        {
            menuShown = true;

            PopupMenu m;
            m.setLookAndFeel (&getLookAndFeel());
            m.addItem (1, TRANS ("velocity-sensitive mode"), true, isVelocityBased);
            m.addSeparator();

            if (style == Rotary || style == RotaryHorizontalDrag || style == RotaryVerticalDrag)
            {
                PopupMenu rotaryMenu;
                rotaryMenu.addItem (2, TRANS ("use circular dragging"), true, style == Rotary);
                rotaryMenu.addItem (3, TRANS ("use left-right dragging"), true, style == RotaryHorizontalDrag);
                rotaryMenu.addItem (4, TRANS ("use up-down dragging"), true, style == RotaryVerticalDrag);

                m.addSubMenu (TRANS ("rotary mode"), rotaryMenu);
            }

            m.showMenuAsync (PopupMenu::Options(),
                             ModalCallbackFunction::forComponent (sliderMenuCallback, this));
        }
        else if (maximum > minimum)
        {
            menuShown = false;

            if (valueBox != nullptr)
                valueBox->hideEditor (true);

            sliderBeingDragged = 0;

            if (style == TwoValueHorizontal
                 || style == TwoValueVertical
                 || style == ThreeValueHorizontal
                 || style == ThreeValueVertical)
            {
                const float mousePos = (float) (isVertical() ? e.y : e.x);

                const float normalPosDistance = std::abs (getLinearMappingSliderPos (currentValue.getValue()) - mousePos);
                const float minPosDistance = std::abs (getLinearMappingSliderPos (valueMin.getValue()) - 0.1f - mousePos);
                const float maxPosDistance = std::abs (getLinearMappingSliderPos (valueMax.getValue()) + 0.1f - mousePos);

                if (style == TwoValueHorizontal || style == TwoValueVertical)
                {
                    if (maxPosDistance <= minPosDistance)
                        sliderBeingDragged = 2;
                    else
                        sliderBeingDragged = 1;
                }
                else if (style == ThreeValueHorizontal || style == ThreeValueVertical)
                {
                    if (normalPosDistance >= minPosDistance && maxPosDistance >= minPosDistance)
                        sliderBeingDragged = 1;
                    else if (normalPosDistance >= maxPosDistance)
                        sliderBeingDragged = 2;
                }
            }

            minMaxDiff = (double) valueMax.getValue() - (double) valueMin.getValue();

            lastAngle = rotaryStart + (rotaryEnd - rotaryStart)
                                        * valueToProportionOfLength (currentValue.getValue());

            valueWhenLastDragged = ((sliderBeingDragged == 2) ? valueMax
                                                              : ((sliderBeingDragged == 1) ? valueMin
                                                                                           : currentValue)).getValue();

            valueOnMouseDown = valueWhenLastDragged;

            if (popupDisplayEnabled)
            {
                PopupDisplayComponent* const popup = new PopupDisplayComponent (*this);
                popupDisplay = popup;

                if (parentForPopupDisplay != nullptr)
                    parentForPopupDisplay->addChildComponent (popup);
                else
                    popup->addToDesktop (0);

                popup->setVisible (true);
            }

            sendDragStart();

            mouseDrag (e);
        }
    }
}

void MappingSlider::mouseUp (const MouseEvent&)
{
    if (isEnabled()
         && (! menuShown)
         && (maximum > minimum)
         && (style != IncDecButtons || incDecDragged))
    {
        restoreMouseIfHidden();

        if (sendChangeOnlyOnRelease && valueOnMouseDown != (double) currentValue.getValue())
            triggerChangeMessage (false);

        sendDragEnd();
        popupDisplay = nullptr;

        if (style == IncDecButtons)
        {
            incButton->setState (Button::buttonNormal);
            decButton->setState (Button::buttonNormal);
        }
    }
    else if (popupDisplay != nullptr)
    {
        popupDisplay->startTimer (2000);
    }
}

void MappingSlider::restoreMouseIfHidden()
{
    if (mouseWasHidden)
    {
        mouseWasHidden = false;

        for (int i = Desktop::getInstance().getNumMouseSources(); --i >= 0;)
            Desktop::getInstance().getMouseSource(i)->enableUnboundedMouseMovement (false);

        const double pos = (sliderBeingDragged == 2) ? getMaxValue()
                                                     : ((sliderBeingDragged == 1) ? getMinValue()
                                                                                  : (double) currentValue.getValue());

        Point<int> mousePos;

        if (style == RotaryHorizontalDrag || style == RotaryVerticalDrag)
        {
            mousePos = Desktop::getLastMouseDownPosition();

            if (style == RotaryHorizontalDrag)
            {
                const double posDiff = valueToProportionOfLength (pos) - valueToProportionOfLength (valueOnMouseDown);
                mousePos += Point<int> (roundToInt (pixelsForFullDragExtent * posDiff), 0);
            }
            else
            {
                const double posDiff = valueToProportionOfLength (valueOnMouseDown) - valueToProportionOfLength (pos);
                mousePos += Point<int> (0, roundToInt (pixelsForFullDragExtent * posDiff));
            }
        }
        else
        {
            const int pixelPos = (int) getLinearMappingSliderPos (pos);

            mousePos = localPointToGlobal (Point<int> (isHorizontal() ? pixelPos : (getWidth() / 2),
                                                       isVertical()   ? pixelPos : (getHeight() / 2)));
        }

        Desktop::setMousePosition (mousePos);
    }
}

void MappingSlider::modifierKeysChanged (const ModifierKeys& modifiers)
{
    if (isEnabled()
         && style != IncDecButtons
         && style != Rotary
         && isVelocityBased == modifiers.isAnyModifierKeyDown())
    {
        restoreMouseIfHidden();
    }
}

namespace MappingSliderHelpers
{
    double smallestAngleBetween (double a1, double a2) noexcept
    {
        return jmin (std::abs (a1 - a2),
                     std::abs (a1 + double_Pi * 2.0 - a2),
                     std::abs (a2 + double_Pi * 2.0 - a1));
    }
}

void MappingSlider::mouseDrag (const MouseEvent& e)
{
    if (isEnabled()
         && (! menuShown)
         && (maximum > minimum))
    {
        if (style == Rotary)
        {
            const int dx = e.x - sliderRect.getCentreX();
            const int dy = e.y - sliderRect.getCentreY();

            if (dx * dx + dy * dy > 25)
            {
                double angle = std::atan2 ((double) dx, (double) -dy);
                while (angle < 0.0)
                    angle += double_Pi * 2.0;

                if (rotaryStop && ! e.mouseWasClicked())
                {
                    if (std::abs (angle - lastAngle) > double_Pi)
                    {
                        if (angle >= lastAngle)
                            angle -= double_Pi * 2.0;
                        else
                            angle += double_Pi * 2.0;
                    }

                    if (angle >= lastAngle)
                        angle = jmin (angle, (double) jmax (rotaryStart, rotaryEnd));
                    else
                        angle = jmax (angle, (double) jmin (rotaryStart, rotaryEnd));
                }
                else
                {
                    while (angle < rotaryStart)
                        angle += double_Pi * 2.0;

                    if (angle > rotaryEnd)
                    {
                        if (MappingSliderHelpers::smallestAngleBetween (angle, rotaryStart)
                             <= MappingSliderHelpers::smallestAngleBetween (angle, rotaryEnd))
                            angle = rotaryStart;
                        else
                            angle = rotaryEnd;
                    }
                }

                const double proportion = (angle - rotaryStart) / (rotaryEnd - rotaryStart);

                valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, proportion));

                lastAngle = angle;
            }
        }
        else
        {
            if (style == LinearBar && e.mouseWasClicked()
                 && valueBox != nullptr && valueBox->isEditable())
                return;

            if (style == IncDecButtons && ! incDecDragged)
            {
                if (e.getDistanceFromDragStart() < 10 || e.mouseWasClicked())
                    return;

                incDecDragged = true;
                mouseDragStartX = e.x;
                mouseDragStartY = e.y;
            }

            if ((isVelocityBased == (userKeyOverridesVelocity ? e.mods.testFlags (ModifierKeys::ctrlModifier | ModifierKeys::commandModifier | ModifierKeys::altModifier)
                                                              : false))
                || ((maximum - minimum) / sliderRegionSize < interval))
            {
                const int mousePos = (isHorizontal() || style == RotaryHorizontalDrag) ? e.x : e.y;

                double scaledMousePos = (mousePos - sliderRegionStart) / (double) sliderRegionSize;

                if (style == RotaryHorizontalDrag
                    || style == RotaryVerticalDrag
                    || style == IncDecButtons
                    || ((style == LinearHorizontal || style == LinearVertical || style == LinearBar)
                        && ! snapsToMousePos))
                {
                    const int mouseDiff = (style == RotaryHorizontalDrag
                                             || style == LinearHorizontal
                                             || style == LinearBar
                                             || (style == IncDecButtons && incDecDragDirectionIsHorizontal()))
                                            ? e.x - mouseDragStartX
                                            : mouseDragStartY - e.y;

                    double newPos = valueToProportionOfLength (valueOnMouseDown)
                                       + mouseDiff * (1.0 / pixelsForFullDragExtent);

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, newPos));

                    if (style == IncDecButtons)
                    {
                        incButton->setState (mouseDiff < 0 ? Button::buttonNormal : Button::buttonDown);
                        decButton->setState (mouseDiff > 0 ? Button::buttonNormal : Button::buttonDown);
                    }
                }
                else
                {
                    if (isVertical())
                        scaledMousePos = 1.0 - scaledMousePos;

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, scaledMousePos));
                }
            }
            else
            {
                const int mouseDiff = (isHorizontal() || style == RotaryHorizontalDrag
                                         || (style == IncDecButtons && incDecDragDirectionIsHorizontal()))
                                        ? e.x - mousePosWhenLastDragged.getX()
                                        : e.y - mousePosWhenLastDragged.getY();

                const double maxSpeed = jmax (200, sliderRegionSize);
                double speed = jlimit (0.0, maxSpeed, (double) abs (mouseDiff));

                if (speed != 0)
                {
                    speed = 0.2 * velocityModeSensitivity
                              * (1.0 + std::sin (double_Pi * (1.5 + jmin (0.5, velocityModeOffset
                                                                            + jmax (0.0, (double) (speed - velocityModeThreshold))
                                                                                / maxSpeed))));

                    if (mouseDiff < 0)
                        speed = -speed;

                    if (isVertical() || style == RotaryVerticalDrag
                         || (style == IncDecButtons && ! incDecDragDirectionIsHorizontal()))
                        speed = -speed;

                    const double currentPos = valueToProportionOfLength (valueWhenLastDragged);

                    valueWhenLastDragged = proportionOfLengthToValue (jlimit (0.0, 1.0, currentPos + speed));

                    e.source.enableUnboundedMouseMovement (true, false);
                    mouseWasHidden = true;
                }
            }
        }

        valueWhenLastDragged = jlimit (minimum, maximum, valueWhenLastDragged);

        if (sliderBeingDragged == 0)
        {
            setValue (snapValue (valueWhenLastDragged, true),
                      ! sendChangeOnlyOnRelease, true);
        }
        else if (sliderBeingDragged == 1)
        {
            setMinValue (snapValue (valueWhenLastDragged, true),
                         ! sendChangeOnlyOnRelease, false, true);

            if (e.mods.isShiftDown())
                setMaxValue (getMinValue() + minMaxDiff, false, false, true);
            else
                minMaxDiff = (double) valueMax.getValue() - (double) valueMin.getValue();
        }
        else
        {
            jassert (sliderBeingDragged == 2);

            setMaxValue (snapValue (valueWhenLastDragged, true),
                         ! sendChangeOnlyOnRelease, false, true);

            if (e.mods.isShiftDown())
                setMinValue (getMaxValue() - minMaxDiff, false, false, true);
            else
                minMaxDiff = (double) valueMax.getValue() - (double) valueMin.getValue();
        }

        mousePosWhenLastDragged = e.getPosition();
    }
}

void MappingSlider::mouseDoubleClick (const MouseEvent&)
{
    if (doubleClickToValue
         && isEnabled()
         && style != IncDecButtons
         && minimum <= doubleClickReturnValue
         && maximum >= doubleClickReturnValue)
    {
        sendDragStart();
        setValue (doubleClickReturnValue, true, true);
        sendDragEnd();
    }
}

void MappingSlider::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    if (scrollWheelEnabled && isEnabled()
         && style != TwoValueHorizontal
         && style != TwoValueVertical)
    {
        if (maximum > minimum && ! e.mods.isAnyMouseButtonDown())
        {
            if (valueBox != nullptr)
                valueBox->hideEditor (false);

            const double value = (double) currentValue.getValue();
			const double proportionDelta = (wheel.deltaX != 0 ? -wheel.deltaX : wheel.deltaY) * 0.15f;
            const double currentPos = valueToProportionOfLength (value);
            const double newValue = proportionOfLengthToValue (jlimit (0.0, 1.0, currentPos + proportionDelta));

            double delta = (newValue != value)
                            ? jmax (std::abs (newValue - value), interval) : 0;

            if (value > newValue)
                delta = -delta;

            sendDragStart();
            setValue (snapValue (value + delta, false), true, true);
            sendDragEnd();
        }
    }
    else
    {
        Component::mouseWheelMove (e, wheel);
    }
}

void MappingSliderListener::sliderDragStarted (MappingSlider*)  // (can't write MappingSlider::Listener due to idiotic VC2005 bug)
{
}

void MappingSliderListener::sliderDragEnded (MappingSlider*)
{
}

class SliderLabelComp : public Label
{
public:
    SliderLabelComp() : Label (String::empty, String::empty) {}

    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel) {}
};

Label* MappingSlider::createMappingSliderTextBox (MappingSlider& slider)
{
    Label* const l = new SliderLabelComp();

    l->setJustificationType (Justification::centred);

    l->setColour (Label::textColourId, slider.findColour (MappingSlider::textBoxTextColourId));

    l->setColour (Label::backgroundColourId,
                  (getMappingSliderStyle() == MappingSlider::LinearBar) ? Colours::transparentBlack
                                                                 : findColour (MappingSlider::textBoxBackgroundColourId));
    l->setColour (Label::outlineColourId, slider.findColour (MappingSlider::textBoxOutlineColourId));

    l->setColour (TextEditor::textColourId, slider.findColour (MappingSlider::textBoxTextColourId));

    l->setColour (TextEditor::backgroundColourId,
                  slider.findColour (MappingSlider::textBoxBackgroundColourId)
                        .withAlpha (getMappingSliderStyle() == MappingSlider::LinearBar ? 0.7f : 1.0f));

    l->setColour (TextEditor::outlineColourId, findColour (MappingSlider::textBoxOutlineColourId));

    return l;
}

Button* MappingSlider::createMappingSliderButton (const bool isIncrement)
{
    return new TextButton (isIncrement ? "+" : "-", String::empty);
}

void MappingSlider::drawRotaryMappingSlider (Graphics& g,
                                    int x, int y,
                                    int width, int height,
                                    float sliderPos,
                                    const float rotaryStartAngle,
                                    const float rotaryEndAngle,
                                    MappingSlider& slider)
{
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    if (radius > 12.0f)
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (MappingSlider::rotaryMappingSliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));

        const float thickness = 0.7f;

        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw,
                                    rotaryStartAngle,
                                    angle,
                                    thickness);

            g.fillPath (filledArc);
        }

        if (thickness > 0)
        {
            const float innerRadius = radius * 0.2f;
            Path p;
            p.addTriangle (-innerRadius, 0.0f,
                           0.0f, -radius * thickness * 1.1f,
                           innerRadius, 0.0f);

            p.addEllipse (-innerRadius, -innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

            g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
        }

        if (slider.isEnabled())
            g.setColour (slider.findColour (MappingSlider::rotaryMappingSliderOutlineColourId));
        else
            g.setColour (Colour (0x80808080));

        Path outlineArc;
        outlineArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
        outlineArc.closeSubPath();

        g.strokePath (outlineArc, PathStrokeType (slider.isEnabled() ? (isMouseOver ? 2.0f : 1.2f) : 0.3f));
    }
    else
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (MappingSlider::rotaryMappingSliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));

        Path p;
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType (rw * 0.1f).createStrokedPath (p, p);

        p.addLineSegment (Line<float> (0.0f, 0.0f, 0.0f, -radius), rw * 0.2f);

        g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
    }
}

const Colour createBaseColour (const Colour& buttonColour,
                                   const bool hasKeyboardFocus,
                                   const bool isMouseOverButton,
                                   const bool isButtonDown) noexcept
    {
        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
        const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

        if (isButtonDown)
            return baseColour.contrasting (0.2f);
        else if (isMouseOverButton)
            return baseColour.contrasting (0.1f);

        return baseColour;
    }

void createRoundedPath (Path& p,
                            const float x, const float y,
                            const float w, const float h,
                            const float cs,
                            const bool curveTopLeft, const bool curveTopRight,
                            const bool curveBottomLeft, const bool curveBottomRight) noexcept
    {
        const float cs2 = 2.0f * cs;

        if (curveTopLeft)
        {
            p.startNewSubPath (x, y + cs);
            p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
        }
        else
        {
            p.startNewSubPath (x, y);
        }

        if (curveTopRight)
        {
            p.lineTo (x + w - cs, y);
            p.addArc (x + w - cs2, y, cs2, cs2, 0.0f, float_Pi * 0.5f);
        }
        else
        {
            p.lineTo (x + w, y);
        }

        if (curveBottomRight)
        {
            p.lineTo (x + w, y + h - cs);
            p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
        }
        else
        {
            p.lineTo (x + w, y + h);
        }

        if (curveBottomLeft)
        {
            p.lineTo (x + cs, y + h);
            p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
        }
        else
        {
            p.lineTo (x, y + h);
        }

        p.closeSubPath();
    }

void drawShinyButtonShape (Graphics& g,
                                        float x, float y, float w, float h,
                                        float maxCornerSize,
                                        const Colour& baseColour,
                                        const float strokeWidth,
                                        const bool flatOnLeft,
                                        const bool flatOnRight,
                                        const bool flatOnTop,
                                        const bool flatOnBottom) noexcept
{
    if (w <= strokeWidth * 1.1f || h <= strokeWidth * 1.1f)
        return;

    const float cs = jmin (maxCornerSize, w * 0.5f, h * 0.5f);

    Path outline;
    createRoundedPath (outline, x, y, w, h, cs,
                                            ! (flatOnLeft || flatOnTop),
                                            ! (flatOnRight || flatOnTop),
                                            ! (flatOnLeft || flatOnBottom),
                                            ! (flatOnRight || flatOnBottom));

    ColourGradient cg (baseColour, 0.0f, y,
                       baseColour.overlaidWith (Colour (0x070000ff)), 0.0f, y + h,
                       false);

    cg.addColour (0.5, baseColour.overlaidWith (Colour (0x33ffffff)));
    cg.addColour (0.51, baseColour.overlaidWith (Colour (0x110000ff)));

    g.setGradientFill (cg);
    g.fillPath (outline);

    g.setColour (Colour (0x80000000));
    g.strokePath (outline, PathStrokeType (strokeWidth));
}

void MappingSlider::drawLinearMappingSlider (Graphics& g,
                                    int x, int y,
                                    int width, int height,
                                    float sliderPos,
                                    float minSliderPos,
                                    float maxSliderPos,
                                    const MappingSlider::MappingSliderStyle style,
                                    MappingSlider& slider)
{
    g.fillAll (slider.findColour (MappingSlider::backgroundColourId));

    if (style == MappingSlider::LinearBar)
    {
        const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        Colour baseColour (createBaseColour (slider.findColour (MappingSlider::thumbColourId)
                                                                       .withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f),
                                                                 false, isMouseOver,
                                                                 isMouseOver || slider.isMouseButtonDown()));

        drawShinyButtonShape (g,
                              (float) x, (float) y, sliderPos - (float) x, (float) height, 0.0f,
                              baseColour,
                              slider.isEnabled() ? 0.9f : 0.3f,
                              true, true, true, true);
    }
    else
    {
        drawLinearMappingSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearMappingSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void MappingSlider::drawLinearMappingSliderBackground (Graphics& g,
                                              int x, int y,
                                              int width, int height,
                                              float /*sliderPos*/,
                                              float /*minSliderPos*/,
                                              float /*maxSliderPos*/,
                                              const MappingSlider::MappingSliderStyle /*style*/,
                                              MappingSlider& slider)
{
    const float sliderRadius = (float) (getMappingSliderThumbRadius (slider) - 2);

    const Colour trackColour (slider.findColour (MappingSlider::trackColourId));
    const Colour gradCol1 (trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f)));
    const Colour gradCol2 (trackColour.overlaidWith (Colour (0x14000000)));
    Path indent;

    if (slider.isHorizontal())
    {
        const float iy = y + height * 0.5f - sliderRadius * 0.5f;
        const float ih = sliderRadius;

        g.setGradientFill (ColourGradient (gradCol1, 0.0f, iy,
                                           gradCol2, 0.0f, iy + ih, false));

        indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy,
                                    width + sliderRadius, ih,
                                    5.0f);
        g.fillPath (indent);
    }
    else
    {
        const float ix = x + width * 0.5f - sliderRadius * 0.5f;
        const float iw = sliderRadius;

        g.setGradientFill (ColourGradient (gradCol1, ix, 0.0f,
                                           gradCol2, ix + iw, 0.0f, false));

        indent.addRoundedRectangle (ix, y - sliderRadius * 0.5f,
                                    iw, height + sliderRadius,
                                    5.0f);
        g.fillPath (indent);
    }

    g.setColour (Colour (0x4c000000));
    g.strokePath (indent, PathStrokeType (0.5f));
}

void drawGlassSphere (Graphics& g,
                                   const float x, const float y,
                                   const float diameter,
                                   const Colour& colour,
                                   const float outlineThickness) noexcept
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.addEllipse (x, y, diameter, diameter);

    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.4, Colours::white.overlaidWith (colour));

        g.setGradientFill (cg);
        g.fillPath (p);
    }

    g.setGradientFill (ColourGradient (Colours::white, 0, y + diameter * 0.06f,
                                       Colours::transparentWhite, 0, y + diameter * 0.3f, false));
    g.fillEllipse (x + diameter * 0.2f, y + diameter * 0.05f, diameter * 0.6f, diameter * 0.4f);

    ColourGradient cg (Colours::transparentBlack,
                       x + diameter * 0.5f, y + diameter * 0.5f,
                       Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                       x, y + diameter * 0.5f, true);

    cg.addColour (0.7, Colours::transparentBlack);
    cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));

    g.setGradientFill (cg);
    g.fillPath (p);

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.drawEllipse (x, y, diameter, diameter, outlineThickness);
}

void drawGlassPointer (Graphics& g,
                                    const float x, const float y,
                                    const float diameter,
                                    const Colour& colour, const float outlineThickness,
                                    const int direction) noexcept
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.startNewSubPath (x + diameter * 0.5f, y);
    p.lineTo (x + diameter, y + diameter * 0.6f);
    p.lineTo (x + diameter, y + diameter);
    p.lineTo (x, y + diameter);
    p.lineTo (x, y + diameter * 0.6f);
    p.closeSubPath();

    p.applyTransform (AffineTransform::rotation (direction * (float_Pi * 0.5f), x + diameter * 0.5f, y + diameter * 0.5f));

    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

        cg.addColour (0.4, Colours::white.overlaidWith (colour));

        g.setGradientFill (cg);
        g.fillPath (p);
    }

    ColourGradient cg (Colours::transparentBlack,
                       x + diameter * 0.5f, y + diameter * 0.5f,
                       Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                       x - diameter * 0.2f, y + diameter * 0.5f, true);

    cg.addColour (0.5, Colours::transparentBlack);
    cg.addColour (0.7, Colours::black.withAlpha (0.07f * outlineThickness));

    g.setGradientFill (cg);
    g.fillPath (p);

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.strokePath (p, PathStrokeType (outlineThickness));
}

void MappingSlider::drawLinearMappingSliderThumb (Graphics& g,
                                         int x, int y,
                                         int width, int height,
                                         float sliderPos,
                                         float minSliderPos,
                                         float maxSliderPos,
                                         const MappingSlider::MappingSliderStyle style,
                                         MappingSlider& slider)
{
    const float sliderRadius = (float) (getMappingSliderThumbRadius (slider) - 2);

    Colour knobColour (createBaseColour (slider.findColour (MappingSlider::thumbColourId),
                                                             slider.hasKeyboardFocus (false) && slider.isEnabled(),
                                                             slider.isMouseOverOrDragging() && slider.isEnabled(),
                                                             slider.isMouseButtonDown() && slider.isEnabled()));

    const float outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;

    if (style == MappingSlider::LinearHorizontal || style == MappingSlider::LinearVertical)
    {
        float kx, ky;

        if (style == MappingSlider::LinearVertical)
        {
            kx = x + width * 0.5f;
            ky = sliderPos;
        }
        else
        {
            kx = sliderPos;
            ky = y + height * 0.5f;
        }

        drawGlassSphere (g,
                         kx - sliderRadius,
                         ky - sliderRadius,
                         sliderRadius * 2.0f,
                         knobColour, outlineThickness);
    }
    else
    {
        if (style == MappingSlider::ThreeValueVertical)
        {
            drawGlassSphere (g, x + width * 0.5f - sliderRadius,
                             sliderPos - sliderRadius,
                             sliderRadius * 2.0f,
                             knobColour, outlineThickness);
        }
        else if (style == MappingSlider::ThreeValueHorizontal)
        {
            drawGlassSphere (g,sliderPos - sliderRadius,
                             y + height * 0.5f - sliderRadius,
                             sliderRadius * 2.0f,
                             knobColour, outlineThickness);
        }

        if (style == MappingSlider::TwoValueVertical || style == MappingSlider::ThreeValueVertical)
        {
            const float sr = jmin (sliderRadius, width * 0.4f);

            drawGlassPointer (g, jmax (0.0f, x + width * 0.5f - sliderRadius * 2.0f),
                              minSliderPos - sliderRadius,
                              sliderRadius * 2.0f, knobColour, outlineThickness, 1);

            drawGlassPointer (g, jmin (x + width - sliderRadius * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                              sliderRadius * 2.0f, knobColour, outlineThickness, 3);
        }
        else if (style == MappingSlider::TwoValueHorizontal || style == MappingSlider::ThreeValueHorizontal)
        {
            const float sr = jmin (sliderRadius, height * 0.4f);

            drawGlassPointer (g, minSliderPos - sr,
                              jmax (0.0f, y + height * 0.5f - sliderRadius * 2.0f),
                              sliderRadius * 2.0f, knobColour, outlineThickness, 2);

            drawGlassPointer (g, maxSliderPos - sliderRadius,
                              jmin (y + height - sliderRadius * 2.0f, y + height * 0.5f),
                              sliderRadius * 2.0f, knobColour, outlineThickness, 4);
        }
    }
}
