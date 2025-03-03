/** @file ngage_appui.cpp
 *
 *  A portable PICO-8 emulator written in C.
 *
 *  Copyright (c) 2025, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <avkon.hrh>
#include <aknnotewrappers.h>

#include "ngage_appui.h"
#include "ngage_appview.h"

void CNGageAppUi::ConstructL()
{
    BaseConstructL();

    iAppView = CNGageAppView::NewL(ClientRect());

    AddToStackL(iAppView);
}

CNGageAppUi::CNGageAppUi()
{
    RProcess Proc;

    iAppView = NULL;

    if (KErrNone == Proc.Create(_L("E:\\System\\Apps\\open8\\game.exe"), _L("")))
    {
        TRequestStatus status;
        Proc.Logon(status);
        Proc.Resume();
        User::WaitForRequest(status);
        Proc.Close();
        Exit();
    }
    else
    {
        Exit();
    }
}

CNGageAppUi::~CNGageAppUi()
{
    if (iAppView)
    {
        RemoveFromStack(iAppView);
        delete iAppView;
        iAppView = NULL;
    }
}

void CNGageAppUi::HandleCommandL(TInt aCommand)
{
    /* No implementation required. */
}
