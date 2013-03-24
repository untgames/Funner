#include <shlobj.h>
#include <stdio.h>
#include <shobjidl.h>
#include <objbase.h>
#include <atlbase.h>
#include <string>

/*++

  Routine Description:

    This routine launches your app using IApplicationActivationManager.

  Arguments:

    strAppUserModelID - AppUserModelID of the app to launch.
    pdwProcessId - Output argument that receives the process id of the launched app.

  Return value:

    HRESULT indicating success/failure

--*/
HRESULT LaunchApp(const std::wstring& strAppUserModelId, PDWORD pdwProcessId)
{
    CComPtr<IApplicationActivationManager> spAppActivationManager;
    HRESULT hrResult = E_INVALIDARG;
    if (!strAppUserModelId.empty())
    {
        // Instantiate IApplicationActivationManager
        hrResult = CoCreateInstance(CLSID_ApplicationActivationManager,
            NULL,
            CLSCTX_LOCAL_SERVER,
            IID_IApplicationActivationManager,
            (LPVOID*)&spAppActivationManager);

        if (SUCCEEDED(hrResult))
        {
            // This call ensures that the app is launched as the foreground window
            hrResult = CoAllowSetForegroundWindow(spAppActivationManager, NULL);
            
            // Launch the app
            if (SUCCEEDED(hrResult))
            {
printf ("%s(%u)\n", __FUNCTION__, __LINE__);
                hrResult = spAppActivationManager->ActivateApplication(strAppUserModelId.c_str(),
                    NULL,
                    AO_NONE,
                    pdwProcessId);
printf ("%s(%u): %08x\n", __FUNCTION__, __LINE__, hrResult);
            }
        }
    }

    return hrResult;
}

int wmain(int argc, wchar_t* argv[])
{
    HRESULT hrResult = S_OK;
    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        if (argc == 2)
        {
            DWORD dwProcessId = 0;
            ++argv;
            hrResult = LaunchApp(*argv, &dwProcessId);
        }
        else
        {
            printf ("usage: %S <AppName>\n", argv [0]);
            hrResult = E_INVALIDARG;
        }

        CoUninitialize();
    }

    return hrResult;
}
