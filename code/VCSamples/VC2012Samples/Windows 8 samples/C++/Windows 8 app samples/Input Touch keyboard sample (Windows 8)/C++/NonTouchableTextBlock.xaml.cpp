﻿// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// NonTouchableTextBlock.xaml.cpp
// Implementation of the NonTouchableTextBlock class
//

#include "pch.h"
#include "NonTouchableTextBlock.xaml.h"

using namespace SDKSample::TouchKeyboard;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Automation::Peers;     // needed for FrameworkElementAutomationPeer class

namespace SDKSample
{
namespace TouchKeyboard
{
/// <summary>
/// Automation Peer class for NonTouchableTextBlock.
///
/// Note: The difference between this and TouchableTextBlockAutomationPeer is that this one does not implement
/// Text Pattern (ITextProvider) and Value Pattern (IValuePattern) interfaces.  So Touch keyboard does not show
/// automatically when user taps on the control with Touch or Pen.
/// </summary>
[Windows::Foundation::Metadata::WebHostHidden]
public ref class NonTouchableTextBlockAutomationPeer sealed : FrameworkElementAutomationPeer
{
public:

    NonTouchableTextBlockAutomationPeer(NonTouchableTextBlock^ owner)
        : FrameworkElementAutomationPeer(owner)
    {
        this->textBlock = owner;
        this->accClass = "NonTouchableTextBlock";
    }
protected:
    /// <summary>
    /// Override GetClassNameCore and set the name of the class that defines the type associated with this control.
    /// </summary>
    /// <returns>The name of the control class</returns>
    virtual String^ GetClassNameCore() override
    {
        return this->accClass;
    }

private:
    NonTouchableTextBlock^ textBlock;
    String^ accClass;

};

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

/// <summary>
/// Loads the XAML UI contents and set properties required for this custom control.
/// </summary>
NonTouchableTextBlock::NonTouchableTextBlock()
{
    InitializeComponent();
    this->IsTabStop = true;
    this->IsTapEnabled = true;
}

/// <summary>
/// Create the Automation peer implementations for custom control (CustomInputBox2) to provide the accessibility support.
/// </summary>
/// <returns>Automation Peer implementation for this control</returns>
AutomationPeer^ NonTouchableTextBlock::OnCreateAutomationPeer()
{
    return ref new NonTouchableTextBlockAutomationPeer(this);
}

/// <summary>
/// Override the default event handler for GotFocus.
/// When the control got focus, indicate it has focus by highlighting the control by changing the background color to yellow.
/// </summary>
/// <param name="e">State information and event data associated with GotFocus event.</param>
void NonTouchableTextBlock::OnGotFocus(RoutedEventArgs^ e)
{
    this->myBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Yellow);
}

/// <summary>
/// Override the default event handler for LostFocus.
/// When the control lost focus, indicate it does not have focus by changing the background color to gray.
/// And the content is cleared.
/// </summary>
/// <param name="e">State information and event data associated with LostFocus event.</param>
void NonTouchableTextBlock::OnLostFocus(RoutedEventArgs^ e)
{
    this->myBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Gray);
    this->myTextBlock->Text = "";
}

/// <summary>
/// Override the default event handler for Tapped.
/// Set input focus to the control when tapped on.
/// </summary>
/// <param name="e">State information and event data associated with Tapped event.</param>
void NonTouchableTextBlock::OnTapped(TappedRoutedEventArgs^ e)
{
    this->Focus(Windows::UI::Xaml::FocusState::Pointer);
}

/// <summary>
/// Override the default event handler for KeyDown.
/// Displays the text "A key is pressed" and the approximate time when the key is pressed.
/// </summary>
/// <param name="e">State information and event data associated with KeyDown event.</param>
void NonTouchableTextBlock::OnKeyDown(KeyRoutedEventArgs^ e)
{
    auto currentCalendar = ref new Windows::Globalization::Calendar();
    String^ timeOfDay = currentCalendar->HourAsPaddedString(2) + ":" +
                        currentCalendar->MinuteAsPaddedString(2) + ":" +
                        currentCalendar->SecondAsPaddedString(2);
    this->myTextBlock->Text = "A key was pressed @ " + timeOfDay + " ";
}
}
}
