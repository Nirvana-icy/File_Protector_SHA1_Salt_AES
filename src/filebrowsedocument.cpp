// FileBrowseDocument.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <akndoc.h>
#include <aknapp.h>
#include <aknappui.h>
#include <eiklbo.h>
#include <aknlists.h>

#include "FileBrowseAppUi.h"
#include "FileBrowseDocument.h"

CFileBrowseDocument::CFileBrowseDocument(CAknApplication& aApp)
:	CAknDocument(aApp)
	{}

CFileBrowseDocument* CFileBrowseDocument::NewL(CAknApplication& aApp)
	{
	CFileBrowseDocument* self = new (ELeave) CFileBrowseDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CFileBrowseDocument::ConstructL()
	{}

CFileBrowseDocument::~CFileBrowseDocument()
	{}

CEikAppUi* CFileBrowseDocument::CreateAppUiL()
	{
	return new(ELeave) CFileBrowseAppUi;
	}


