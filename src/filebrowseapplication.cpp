// FileBrowseApplication.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <akndoc.h>
#include <aknapp.h>
#include <EikStart.h>
#include "FileBrowseDocument.h"
#include "FileBrowseApplication.h"
#include "FileBrowseGlobals.h"


CApaDocument* CFileBrowseApplication::CreateDocumentL()
  {
  return CFileBrowseDocument::NewL(*this);
  }

TUid CFileBrowseApplication::AppDllUid() const
  {
  return KUidFileBrowseID;
  }

EXPORT_C CApaApplication* CFileBrowseApplication::NewApplication()
  {
  return new CFileBrowseApplication;
  }

LOCAL_C CApaApplication* NewApplication()
  {
  return new CFileBrowseApplication;
  }

GLDEF_C TInt E32Main()
  {
  return EikStart::RunApplication(NewApplication);
  }


