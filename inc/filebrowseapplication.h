// FileBrowseApplication.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEAPPLICATION_H__
#define __FILEBROWSEAPPLICATION_H__


class CFileBrowseApplication : public CAknApplication
	{
public:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	IMPORT_C CApaApplication* NewApplication();
	};

#endif // __FILEBROWSEAPPLICATION_H__

