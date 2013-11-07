// FileBrowseDocument.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEDOCUMENT_H__
#define __FILEBROWSEDOCUMENT_H__

class CFileBrowseDocument : public CAknDocument
	{
public:
	static CFileBrowseDocument* NewL(CAknApplication& aAppUi);
	~CFileBrowseDocument();
    CEikAppUi* CreateAppUiL();
private:
	CFileBrowseDocument(CAknApplication& aApp);
	void ConstructL();
	};


#endif // __FILEBROWSEDOCUMENT_H__

