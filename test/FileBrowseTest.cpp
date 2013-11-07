//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <e32base.h>
#include <e32cons.h>
#include <e32debug.h>
#include <f32file.h>
#include "rfsengine.h"

_LIT(KFileBrowseTest, "FileBrowseTest");
_LIT(KTxtPressAnyKey, " [Press any key]");
_LIT(KBackSlash, "\\");
_LIT(KCDrive, "C:\\");
_LIT(KPermissionDenied, "Permission Denied");

// Initialises the console and calls RunTestsL()
LOCAL_C void CallExampleL(); 

// Runs the tests
LOCAL_C void RunTestsL(); 

// Recursive function to list files and directories
LOCAL_C void ListDirectoriesL(const TDesC& aPath);

// Allocates a heap buffer from aPath and aSubdir, with backslash terminator
LOCAL_C HBufC* SubDirPathLC(const TDesC& aPath, const TDesC& aSubDir);

LOCAL_D CConsoleBase* console; 

GLDEF_C TInt E32Main() 
    {
	__UHEAP_MARK; // __UHEAP_MARKEND will later check that no memory is left on the heap

	// Create the cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();  
	TRAPD(result, CallExampleL()); 
	// Check that no leave occurred, indicating a test failure
	__ASSERT_ALWAYS(result==KErrNone, User::Panic(KFileBrowseTest, result));
	
	// Destroy the cleanup stack
	delete cleanup;  
	
	__UHEAP_MARKEND;
	return 0; 
    }

LOCAL_C void CallExampleL() 
    {
	console = Console::NewL(KFileBrowseTest, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);

	RunTestsL(); // Run the tests
	
//	console->Printf(KTxtPressAnyKey);
//	console->Getch(); // get and ignore character
	CleanupStack::PopAndDestroy(); // close console 
    }

LOCAL_C void RunTestsL()
    {
	__ASSERT_ALWAYS(console, User::Panic(KFileBrowseTest, KErrNotReady));

	ListDirectoriesL(KCDrive);
	}

// Recursive function to list files and directories
LOCAL_C void ListDirectoriesL(const TDesC& aPath)
	{
	CRFsEngine* engine = CRFsEngine::NewL();
	CleanupStack::PushL(engine);
	
	RDebug::Print(aPath);
	
	TInt result = engine->GetDirectoryAndFileList(aPath);
	
	if (result==KErrNone)
        {	    
	   	TInt dirCount = engine->DirectoryCount();
    	TInt index = 0;
    	
    	for (; index<dirCount; index++)
    		{// Get directories
    	    HBufC* subDirPath = SubDirPathLC(aPath, engine->DirectoryName(index));
    		ListDirectoriesL(*subDirPath);
    		CleanupStack::PopAndDestroy(subDirPath);
    		}

    	// List files in the directory
    	TInt fileCount = engine->FileCount();
    	index = 0;
    	for (; index<fileCount; index++)
    		{
    		RDebug::Print(engine->FileName(index));
    		}
	    }	
	else if (result==KErrPermissionDenied)
    	{    
    	RDebug::Print(KPermissionDenied);
    	}
    else
        User::Leave(result);	    
	
	CleanupStack::PopAndDestroy(engine);	
	
	}
	
LOCAL_C HBufC* SubDirPathLC(const TDesC& aPath, const TDesC& aSubDir)
    {
    HBufC* subDirBuf = HBufC::NewLC(aPath.Length() + aSubDir.Length() +1);
	TPtr subDirPtr(subDirBuf->Des());
	subDirPtr.Copy(aPath);
	subDirPtr.Append(aSubDir);
	subDirPtr.Append(KBackSlash);
	return (subDirBuf);
	}
