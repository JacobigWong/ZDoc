﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// App.xaml.h
// Declaration of the App.xaml class.
//

#pragma once

#include "pch.h"
#include "App.g.h"
#include "MainPage.g.h"

namespace SimpleCommunication
{
ref class App
{
internal:
    App();

public:
    virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ pArgs) override;
    virtual void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ pArgs);

private:
    void SetupWindowing(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ pArgs);
};
}


