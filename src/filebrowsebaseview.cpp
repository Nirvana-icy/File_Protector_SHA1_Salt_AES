// FileBrowseBaseView.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//
#include <EIKAPP.H>
#include <EIKENV.H>
#include <coecntrl.h>
#include <eiklbo.h> 
#include <eikclb.h>
#include <aknlists.h>
#include <BARSREAD.H>
#include <stringloader.h>
#include <gulicon.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <aknnotewrappers.h> 
#include <e32cmn.h>
#include <AVKON.RSG>
#include <filebrowseapp.rsg>
#include "FileBrowseGlobals.h"
#include "FileBrowse.hrh"
#include <filebrowseapp.mbg>
#include "FileBrowseBaseView.h"
#include "rfsengine.h"

_LIT (KListFormatString, "%d\t");

const TInt KFolderIconSlot = 0;
const TInt KFileIconSlot = 1;

CFileBrowseBaseView* CFileBrowseBaseView::NewL(const TRect& aRect)
	{
	CFileBrowseBaseView* me = new (ELeave) CFileBrowseBaseView();
	CleanupStack::PushL(me);
	me->ConstructL(aRect);
	CleanupStack::Pop(me);
	return (me);
	}

CFileBrowseBaseView::CFileBrowseBaseView()
	: iRoot(ETrue)
	{}

CFileBrowseBaseView::~CFileBrowseBaseView()
	{
	delete iEngine;
	iFocusPos.Close();
	delete iListBox;
	}

void CFileBrowseBaseView::ConstructL(const TRect& aRect)
	{
	CreateWindowL();
		
	iEngine = CRFsEngine::NewL();
	
	SetUpListBoxL();
	ShowRootL();

	SetRect(aRect);
	ActivateL();
	}

void CFileBrowseBaseView::SetUpListBoxL()
    {
    iListBox = new (ELeave) CAknSingleGraphicStyleListBox();
	iListBox->SetContainerWindowL(*this);
	
	TResourceReader reader;
	iEikonEnv->CreateResourceReaderLC(reader, R_FILEBROWSE_BASEVIEW_LISTBOX);

	// Create the list box
	iListBox->ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // reader
	
	
	// Add this to observe the list box
	iListBox->SetListBoxObserver(this);
	
	SetupListIconsL();
	
	//  Add vertical scroll bars (which are visible when necessary)
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);	
    }


void CFileBrowseBaseView::SetupListIconsL()
	{
	HBufC* iconFileName;
	iconFileName = StringLoader::LoadLC(R_FILEBROWSE_ICONS);
	CArrayPtr<CGulIcon>* icons = new (ELeave) CAknIconArray(2); // 2 icons
	
	CleanupStack::PushL(icons);
    
    // KFolderIconSlot
    icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmFilebrowseappFolder, EMbmFilebrowseappMask));
    // KFileIconSlot
    icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmFilebrowseappFile, EMbmFilebrowseappMask));

    CleanupStack::Pop(icons);
    CleanupStack::PopAndDestroy(iconFileName);
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	}


// This adds the C and D drives
void CFileBrowseBaseView::ShowRootL()
	{
	// Get the list box model
	CTextListBoxModel* model = iListBox->Model();    // Does not own the returned model
	User::LeaveIfNull(model);
	model->SetOwnershipType(ELbmOwnsItemArray);
	
	CDesCArray* itemArray = static_cast<CDesCArray*>(model->ItemTextArray());
	User::LeaveIfNull(itemArray);
	
	itemArray->Reset(); //  Remove content from the array
	
	TBuf<20> driveBuf;
	CCoeEnv::Static()->ReadResource(driveBuf, R_FILEBROWSE_DRIVEC);
    	
    TBuf<30> string;
    string.Format(KListFormatString, KFolderIconSlot);
    string.Append(driveBuf);
    	
	itemArray->AppendL(string);
	iListBox->HandleItemAdditionL();

	driveBuf.Zero();
	string.Zero();
	CCoeEnv::Static()->ReadResource(driveBuf, R_FILEBROWSE_DRIVED);
	
	string.Format(KListFormatString, KFolderIconSlot);
    string.Append(driveBuf);
    	
    itemArray->AppendL(string);
	iListBox->HandleItemAdditionL();
	
	driveBuf.Zero();
	string.Zero();
	CCoeEnv::Static()->ReadResource(driveBuf, R_FILEBROWSE_DRIVEE);
	
	string.Format(KListFormatString, KFolderIconSlot);
    string.Append(driveBuf);
    	
    itemArray->AppendL(string);
	iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(0);
	iListBox->DrawNow();
	
	iFilePosition = 3; // No files at root level
	iCurrPos = 0;	
	}
	
// This method updates the position, and focus position	and iPath
void CFileBrowseBaseView::NavigateL(TInt aIndex)
	{
	if (iRoot) // Currently in the root directory, set the correct drive to open
    	{
    	if (aIndex==0)
    		{
    		_LIT(KPathC, "C:\\");
    		iPath = KPathC();
    		}
    	else if (aIndex==1)
    		{
    		_LIT(KPathD, "D:\\");
    		iPath = KPathD();
    		}
    	else if (aIndex==2)
    		{
    		_LIT(KPathE, "E:\\");
    		iPath = KPathE();
    		}
    	iRoot = EFalse;
    	}
	else 
    	{
    	if (aIndex == 0) // going back up the folder list
    		{
    		// delete last '\\'
    		iPath.SetLength(iPath.Length()-1);
    		// find and remove last folder name
    		TInt pos = iPath.LocateReverse('\\');
    		
    		// check if this was not the last folder or an error
    		// if so, go back to root
    		if (pos < 2 || pos == KErrNotFound)
    			{
    			iPath.Zero();
    			iRoot = ETrue;
    			}
    		else 
    			{
    			iCurrPos = iFocusPos.operator[](iFocusPos.Count()-1);
    			iFocusPos.Remove(iFocusPos.Count()-1);
    			iPath.SetLength(pos+1);
    			}
    		}
    	else if (aIndex < iFilePosition) // going down the folder hierarchy
    		{
    		iRoot = EFalse;
    		
    		CTextListBoxModel* model = iListBox->Model(); // Does not own the returned model
    	    User::LeaveIfNull(model);
    		TPtrC newPath(model->ItemText(aIndex));
    		
    	//  The path in newPath is prefixed with KListFormatString ("%d\t")
    	    
    		iPath.Append(newPath.Right(newPath.Length()-2));
    		iPath.Append('\\');
    		
    		iCurrPos = 0;
    		iFocusPos.Append(aIndex);
    		}
    	else
        	{
        	HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE);
        	
        	CAknInformationNote* note = new (ELeave) CAknInformationNote();
		    note->ExecuteLD(*noteBuf);
		    CleanupStack::PopAndDestroy(noteBuf);
        	iCurrPos = aIndex;
        	}
        }
	}

void CFileBrowseBaseView::OpenFolderL()
	{
	if (iRoot) //  The root directory view is handled separately
	    {
	    ShowRootL();
	    }
	else
    	{    
    	CTextListBoxModel* model = iListBox->Model();    // Does not own the returned model
    	User::LeaveIfNull(model);
    	
    	model->SetOwnershipType(ELbmOwnsItemArray);
    	
    	CDesCArray* itemArray = static_cast<CDesCArray*>(model->ItemTextArray());
    	User::LeaveIfNull(itemArray);
    	
    	itemArray->Reset(); //  Remove content from the array
    	
    	//  create the first, "back", element
        TBuf<10> backString;
        backString.Format(KListFormatString, KFolderIconSlot);
        _LIT(KBackString, "/..");
        backString.Append(KBackString());
       	
       	itemArray->AppendL(backString);
    	iListBox->HandleItemAdditionL();
    	
    	iFilePosition = 1;
      
        // Now get the subdirectories and files in this directory
    	
    	TInt result = iEngine->GetDirectoryAndFileList(iPath);
      
    	if (result==KErrNone)
    		{
    		TInt dirCount = iEngine->DirectoryCount();
    		TInt index = 0;
    		TFileName path;
    		for (; index <dirCount; ++index)
    			{
    			path.Format(KListFormatString, KFolderIconSlot);
    			path.Append(iEngine->DirectoryName(index));
    			itemArray->AppendL(path);
    			iListBox->HandleItemAdditionL();
    			path.Zero();
    			}
    		    		
            iFilePosition += dirCount;
        	TInt fileCount = iEngine->FileCount();
        	index = 0;
        	for(; index <fileCount; ++index)
        		{
        		path.Format(KListFormatString, KFileIconSlot);
    			path.Append(iEngine->FileName(index));
    			itemArray->AppendL(path);
    			iListBox->HandleItemAdditionL();
    			path.Zero();
        		}
            }
    	
    	if (iCurrPos > model->NumberOfItems() ) 
    		{// in case file number got changed in between
    		iCurrPos = model->NumberOfItems();
    		}
      
    	iListBox->SetCurrentItemIndex(iCurrPos);
    	iListBox->DrawNow();
    	}
	}

void CFileBrowseBaseView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
	{
	switch(aEventType)
		{
		case EEventEnterKeyPressed:
		case EEventItemClicked:
			{// An item has been chosen and will be opened
			NavigateL(iListBox->CurrentItemIndex());
		    OpenFolderL();
			}
		    break;
        default: // Nothing to do
		    break;
    };
  }

TKeyResponse CFileBrowseBaseView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	return (iListBox->OfferKeyEventL(aKeyEvent, aType));
	}
	

void CFileBrowseBaseView::CmdOpenL()
    {
    NavigateL(iListBox->CurrentItemIndex());
    OpenFolderL();
    }

TBool CFileBrowseBaseView::CmdBackL()
    {
    //  If not at the root, go back to the parent folder
    //  If at the root, return False, indicating 
    //  that the app ui should exit the application
    if (!iRoot) 
        {
        NavigateL(0); // Set index = 0 to go back
        OpenFolderL();
        return ETrue;
        }
	return EFalse;
    }
      
void CFileBrowseBaseView::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
	}


void CFileBrowseBaseView::SizeChanged()
	{
	const TPoint listPosition(0,0);
	iListBox->SetExtent(listPosition, iListBox->MinimumSize());
	}

TInt CFileBrowseBaseView::CountComponentControls() const
	{
	return (1); // return number of controls inside this container
	}

CCoeControl* CFileBrowseBaseView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case 0:
			return (iListBox);
		default:
			return (NULL);
		}
	}

void CFileBrowseBaseView::GetPWandDoEncrypt()
	{
	TBuf16<10> iKey_1;			// save pw   
	TBuf16<10> iKey_2;			// save pw for confirming
	CAknTextQueryDialog* iGetPWDlg_1 = new (ELeave)CAknTextQueryDialog(iKey_1, CAknQueryDialog::EConfirmationTone);
//	iGetPWDlg_1->SetDefaultInputMode(EAknEditorNumericInputMode);		//设置默认输入法
	if(EEikBidOk == iGetPWDlg_1->ExecuteLD(R_SET_PW_DLG))   //get the pw in iKey_1
		{
		CAknTextQueryDialog* iGetPWDlg_2 = new (ELeave)CAknTextQueryDialog(iKey_2, CAknQueryDialog::EConfirmationTone);
		if(EEikBidOk == iGetPWDlg_2->ExecuteLD(R_SET_PW_DLG_FOR_CONFIRM))
			{
			if(0 == iKey_1.Compare(iKey_2))  //iKey_1 equal to iKey_2
				{
				TInt iIndex = iListBox->CurrentItemIndex();
				iEngine->DoEncryption(iKey_1, iPath,iIndex);	//do encryption
				}
			else 
				{
				HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_PW_NO_EQUAL);

				CAknInformationNote* note = new (ELeave) CAknInformationNote();
				note->ExecuteLD(*noteBuf);
				CleanupStack::PopAndDestroy(noteBuf);
				}
			}
		}
	}

void CFileBrowseBaseView::GetPWandDoDecrypt()
	{
	TBuf16<10> iKey;
	CAknTextQueryDialog* iGetPWDlg = new (ELeave)CAknTextQueryDialog(iKey, CAknQueryDialog::EConfirmationTone);
//	iGetPWDlg->SetDefaultInputMode(EAknEditorNumericInputMode);
	if(EEikBidOk == iGetPWDlg->ExecuteLD(R_SET_PW_DLG))   //get the pw iKey
		{
		TInt iIndex = iListBox->CurrentItemIndex();
		iEngine->DoDecryption(iKey, iPath, iIndex);	//do decryption
		}
	}

TBool CFileBrowseBaseView::SelectedIsFile()
	{
	if(iListBox->CurrentItemIndex() == 0 || iListBox->CurrentItemIndex() < iFilePosition || iRoot)
		{
		HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_NOT_A_FILE);

		CAknInformationNote* note = new (ELeave) CAknInformationNote();
		note->ExecuteLD(*noteBuf);
		CleanupStack::PopAndDestroy(noteBuf);
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}
