// FileBrowseBaseView.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEBASEVIEW_H__
#define __FILEBROWSEBASEVIEW_H__

class CRFsEngine;

class CFileBrowseBaseView : public CCoeControl, public MEikListBoxObserver
	{
public:
	static CFileBrowseBaseView* NewL(const TRect& aRect);
	~CFileBrowseBaseView();
public:	//	from CCoeControl
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
public: // from MEikListBoxObserver 
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);  
public: //  Handle change directory commands from the soft keys
    void CmdOpenL();
    TBool CmdBackL();
    TBool SelectedIsFile();
    void GetPWandDoEncrypt();
    void GetPWandDoDecrypt();
protected:  //  From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
private:
	void SetUpListBoxL();
	void SetupListIconsL();
	void ShowRootL();
	void NavigateL(TInt aIndex);
	void OpenFolderL();
private:
	CFileBrowseBaseView();
	void ConstructL(const TRect& aRect);
private:	
	void Draw(const TRect& aRect) const;
private:
	CRFsEngine* iEngine;
	CAknColumnListBox* iListBox;
    RArray<TInt> iFocusPos; // position of focus in listbox for the folders passed, used whem going back
private:	
	TFileName iPath;
	TBool iRoot;			// if this is ETrue, the root directories 'C' and 'D' are shown
	TInt iFilePosition;		// marks where files are displayed in the directory
	TInt iCurrPos;          // index in current folder/listbox 
							// 0 - iFilePosition -> a directory is currently selected 
							// > iFilePosition  - a file is currently selected
	};




#endif // __FILEBROWSEBASEVIEW_H__


