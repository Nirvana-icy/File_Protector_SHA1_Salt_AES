// FileBrowseEngine.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <f32file.h>
#include <e32cmn.h>
#include <AknNoteWrappers.h>
#include <StringLoader.h>
#include <utf.h>
#include <hash.h>
#include <cryptosymmetric.h>
#include "RFsEngine.h"
#include "filebrowseapp.rsg"

CRFsEngine* CRFsEngine::NewL()
	{
	CRFsEngine* me = new (ELeave) CRFsEngine();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);	
	return (me);		
	}
	
	
CRFsEngine::~CRFsEngine() 
	{
	delete iFileList;
	delete iDirList;
	iFs.Close();
	}


TInt CRFsEngine::GetDirectoryAndFileList(const TDesC& aPath)
	{
	if (iFileList)
		{
		delete iFileList;
		iFileList = NULL;
		}
	if (iDirList)
		{
		delete iDirList;
		iDirList = NULL;
		}
	
	TInt result = iFs.GetDir(aPath,
                             KEntryAttNormal | KEntryAttHidden | KEntryAttSystem,
                             ESortByName | EDirsFirst | EAscending,
                             iFileList,
                             iDirList);
	return (result);
	}
	
TInt CRFsEngine::DirectoryCount()
	{
	if (iDirList)
		return (iDirList->Count());
	else
		return (0);
	}
	
	
TInt CRFsEngine::FileCount()
	{
	if (iFileList)
		return (iFileList->Count());
	else
		return (0);
	}
	
	
const TDesC& CRFsEngine::DirectoryName(TInt aDirListIndex)
	{
	if ( (!iDirList) || (iDirList->Count()<=aDirListIndex) )
		return (KNullDesC);
	
	else
		return (iDirList->operator[](aDirListIndex).iName);
	
	}

const TDesC& CRFsEngine::FileName(TInt aFileNameIndex)
	{
	if ( (!iFileList) || (iFileList->Count()<= aFileNameIndex) )
		return (KNullDesC);
	
	else
		return (iFileList->operator[](aFileNameIndex).iName);
	}

void CRFsEngine::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}


CRFsEngine::CRFsEngine()
	{}

void CRFsEngine::DoEncryption(TDesC16& aKey, TDesC& aPath ,TInt& aIndex)
	{
	TFileName fn;
	fn.Append(aPath);
	TInt iNumOfDir = iDirList->Count();
	fn.Append(iFileList->operator[](aIndex - iNumOfDir - 1).iName);
	TInt iPos = 0;
	TBuf8<16> gKey;
	this->GeneralKey(aKey, gKey);
	CAESEncryptor* DoAES = CAESEncryptor::NewL(gKey);
	TInt iBlockSize = DoAES->BlockSize();
	HBufC8* buf = HBufC8::NewL(iBlockSize);
	TPtr8 bufPtr = buf->Des();
	RFile rFile;
	User::LeaveIfError(rFile.Open(iFs, fn, EFileRead | EFileWrite));
	do
		{
		if(KErrNone == rFile.Read(iPos, bufPtr, iBlockSize))
			{
			if(0 != buf->Size())      	//buf->Size() == 0 means iPos is already at the end of the file. 
				{						
				if(iBlockSize > buf->Size())  //如果最后一轮读入的数据不足iBlockSize 则其余下的内存覆盖为 whitespace 
					bufPtr.AppendFill(' ', iBlockSize - buf->Size());
				DoAES->Transform(bufPtr);
				rFile.Write(iPos, bufPtr, iBlockSize);
				iPos += iBlockSize;
				}
			}
		else
			{
			HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_READ_FILE_ERROR);
			//File read error
			CAknInformationNote* note = new (ELeave) CAknInformationNote();
			note->ExecuteLD(*noteBuf);
			CleanupStack::PopAndDestroy(noteBuf);
			break;
			}
		}
	while(buf->Size() == iBlockSize); //if iDes.iLength < KLen exit the while
					             //if the last time read KLen bytes do more onece cycle but do nothing
	rFile.Flush();
	TFileName iFileName;
	if (KErrNone  == rFile.FullName(iFileName))
		{
		iFileName.Append(_L(".enc"));
		rFile.Rename(iFileName);	//加密完成后改变文件名为  原文文件名.icy
		}
	rFile.Close();
	delete buf;
	delete DoAES;
	//Show end of encryption note
	HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_END_ENCRYPT);

	CAknInformationNote* note = new (ELeave) CAknInformationNote();
	note->ExecuteLD(*noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);
	}

void CRFsEngine::DoDecryption(TDesC16& aKey, TDesC& aPath ,TInt& aIndex)
	{
	TFileName fn;
	fn.Append(aPath);
	TInt iNumOfDir = iDirList->Count();
	fn.Append(iFileList->operator[](aIndex - iNumOfDir - 1).iName);
	TInt iPos = 0;
	TBuf8<16> gKey;
	this->GeneralKey(aKey, gKey);
	CAESDecryptor* DoAES = CAESDecryptor::NewL(gKey);
	TInt iBlockSize = DoAES->BlockSize();
	HBufC8* buf = HBufC8::NewL(iBlockSize);
	TPtr8 bufPtr = buf->Des();
	RFile rFile;
	User::LeaveIfError(rFile.Open(iFs, fn, EFileRead | EFileWrite));
	do
		{
		if(KErrNone == rFile.Read(iPos, bufPtr, iBlockSize))
			{
			if(0 != buf->Size())      	//buf->Size() == 0 means iPos is already at the end of the file. 
				{
				DoAES->Transform(bufPtr);
				rFile.Write(iPos, bufPtr, iBlockSize);
				iPos += iBlockSize;
				}
			else
				{
				TInt iSize = 0;
				rFile.Read(iPos - iBlockSize, bufPtr, iBlockSize);
				bufPtr.TrimRight();
				if (KErrNone == rFile.Size(iSize))
					rFile.SetSize(iSize - (iBlockSize - buf->Size()));
				rFile.Read(iPos, bufPtr, iBlockSize);	//置buf->Size()为0 退出循环
				}
			}
		else
			{
			HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_READ_FILE_ERROR);
			//File read error
			CAknInformationNote* note = new (ELeave) CAknInformationNote();
			note->ExecuteLD(*noteBuf);
			CleanupStack::PopAndDestroy(noteBuf);
			break;
			}
		}
	while(buf->Size() == iBlockSize); //if iDes.iLength < KLen exit the while
					             //if the last time read KLen bytes do more onece cycle but do nothing
	rFile.Flush();
	TFileName iFileName(_L(""));
	if (KErrNone  == rFile.FullName(iFileName))
		{
		TPtrC16 ptr = iFileName.Left(iFileName.Find(_L(".enc")));
		rFile.Rename(ptr);	//解密完成后 去掉自定义文件名后缀 (.enc)
		}
	rFile.Close();
	delete buf;
	delete DoAES;
	//Show end of encryption note
	HBufC* noteBuf = StringLoader::LoadLC(R_FILEBROWSE_INFONOTE_END_DECRYPT);

	CAknInformationNote* note = new (ELeave) CAknInformationNote();
	note->ExecuteLD(*noteBuf);
	CleanupStack::PopAndDestroy(noteBuf);
	}

void CRFsEngine::GeneralKey(TDesC16& aKey, TDes8& gKey)
	{
	_LIT8(iSalt_1, "@@Nirvana$$"); //iSalt_1 11 char * 8 bit = 88 bit
	_LIT8(iSalt_2, "~Icy*");       //iSalt_2 5 char * 8 bit = 40 bit  
	TBuf8<10> iKey;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iKey, aKey);
	CSHA1* DoDigest = CSHA1::NewL();
	DoDigest->Update(iKey);
	DoDigest->Update(iSalt_1);
	DoDigest->Update(iKey);			//message is consisted by 3 iKey, iSalt_1 and iSalt_2.  
	DoDigest->Update(iSalt_2);		//Total 3*80 + 88 + 40 = 368 bit with 128 bit salt.
	DoDigest->Update(iKey);	
	TPtrC8 iDigest = DoDigest->Final();  //进行摘要运算
	gKey.Copy(iDigest.Ptr(), 16);		 ////取摘要值的前128(16*8)bit作为AES算法的密钥	
	delete DoDigest;
	}

	
