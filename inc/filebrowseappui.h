// FileBrowseAppUi.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEAPPUI_H__
#define __FILEBROWSEAPPUI_H__

#include <aknappui.h> 
#include "FileBrowseBaseView.h"
	
class CFileBrowseBaseView;

class CFileBrowseAppUi : public CAknAppUi
	{
public:
	void ConstructL();
	~CFileBrowseAppUi();
	void HandleCommandL(TInt aCommand);
private:
	CFileBrowseBaseView* iBaseView;
	};


#endif // __FILEBROWSEAPPUI_H__



