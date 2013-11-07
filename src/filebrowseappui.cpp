// FileBrowseAppUi.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
// 
#include <akndialog.h>
#include <eiklbo.h>
#include <aknlists.h>
#include <e32def.h>

#include <FileBrowseapp.rsg>
#include "FileBrowseAppUi.h"
#include "FileBrowse.hrh"

void CFileBrowseAppUi::ConstructL()
  {
  CAknAppUi::BaseConstructL(EAknEnableSkin);
  iBaseView = CFileBrowseBaseView::NewL(ClientRect());
  iBaseView->SetMopParent(this);
  AddToStackL(iBaseView);
  }

CFileBrowseAppUi::~CFileBrowseAppUi()
	{
	if (iBaseView)
		{
		RemoveFromStack(iBaseView);
		delete iBaseView;
		}
	}

void CFileBrowseAppUi::HandleCommandL(TInt aCommand)
  {
  ASSERT(iBaseView);
  switch (aCommand)
    {
    case ECmdFileBrowseExit:
    case EEikCmdExit:
        Exit();
        break;
    case ECmdFileBrowseOpen:
        iBaseView->CmdOpenL();
        break;
    case EAknSoftkeyBack:
        TBool goback = iBaseView->CmdBackL();
        if (!goback) Exit();
        break;
    case ECmdFileBrowseEncrypt:
    	if(iBaseView->SelectedIsFile())
    		iBaseView->GetPWandDoEncrypt();
    	break;
    case ECmdFileBrowseDecrypt:
    	if(iBaseView->SelectedIsFile())
    		iBaseView->GetPWandDoDecrypt();
    	break;
    default:
        CAknAppUi::HandleCommandL(aCommand);
        break;
    }
  }



