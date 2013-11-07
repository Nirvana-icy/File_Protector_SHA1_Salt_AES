// Engine.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __RFSENGINE_H__
#define __RFSENGINE_H__

#include <e32base.h> 

class CRFsEngine : public CBase
	{
public:
	static CRFsEngine* NewL(); 
	~CRFsEngine();
public:
	TInt GetDirectoryAndFileList(const TDesC& aPath);
	TInt DirectoryCount();
	TInt FileCount();
	const TDesC& DirectoryName(TInt aDirListIndex);
	const TDesC& FileName(TInt aFileNameIndex);
	void DoEncryption(TDesC16& aKey, TDesC& aPath, TInt& aIndex);
	void DoDecryption(TDesC16& aKey, TDesC& aPath, TInt& aIndex);
	void GeneralKey(TDesC16& aKey, TDes8& gKey);
private:
	void ConstructL();
	CRFsEngine();
private:
	RFs iFs;
	CDir* iFileList;
	CDir* iDirList;
	};

#endif // __RFSENGINE_H__


