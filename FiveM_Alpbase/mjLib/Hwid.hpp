class Hwid {
private:
    static std::string GetWMIProperty(const std::wstring& className, const std::wstring& propertyName) {
        HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            return "";
        }

        IWbemLocator* pLocator = NULL;
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
        if (FAILED(hres)) {
            CoUninitialize();
            return "";
        }

        IWbemServices* pServices = NULL;
        hres = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, NULL, 0, NULL, NULL, &pServices);
        if (FAILED(hres)) {
            pLocator->Release();
            CoUninitialize();
            return "";
        }

        hres = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        if (FAILED(hres)) {
            pServices->Release();
            pLocator->Release();
            CoUninitialize();
            return "";
        }

        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pServices->ExecQuery(bstr_t("WQL"), bstr_t((L"SELECT " + propertyName + L" FROM " + className).c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
        if (FAILED(hres)) {
            pServices->Release();
            pLocator->Release();
            CoUninitialize();
            return "";
        }

        IWbemClassObject* pObject = NULL;
        ULONG uReturn = 0;
        std::string result;

        while (pEnumerator) {
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pObject, &uReturn);
            if (0 == uReturn) {
                break;
            }

            VARIANT vtProp;
            hr = pObject->Get(propertyName.c_str(), 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
                result = _bstr_t(vtProp.bstrVal);
            }

            VariantClear(&vtProp);
            pObject->Release();
        }

        pEnumerator->Release();
        pServices->Release();
        pLocator->Release();
        CoUninitialize();
        return result;
    }
public:
    static std::string GetMotherboardSerialNumber() {
        return String::Trim(GetWMIProperty(L"Win32_BaseBoard", L"SerialNumber"));
    }

    static std::string GetUUID() {
        return String::Trim(GetWMIProperty(L"Win32_ComputerSystemProduct", L"UUID"));
    }
};