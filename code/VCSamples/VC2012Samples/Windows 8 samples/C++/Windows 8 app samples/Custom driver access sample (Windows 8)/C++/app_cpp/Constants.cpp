﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"
#include "Constants.h"

using namespace SDKSample;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    // The format here is the following:
    //     { "Description for the sample", "Fully quaified name for the class that implements the scenario" }
    { "Connecting to the Fx2 Device", "SDKSample.CustomDeviceAccess.DeviceConnect" },
    { "Getting and Setting device properties", "SDKSample.CustomDeviceAccess.DeviceProperties" },
    { "Registering for device events", "SDKSample.CustomDeviceAccess.DeviceEvents" },
    { "Invoking asynchronous device methods", "SDKSample.CustomDeviceAccess.DeviceAsync" }
};
