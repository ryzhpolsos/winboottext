#include <phnt_windows.h>
#include <phnt.h>

#define MIN_TIMEOUT 10
#define MS_TO_NT_TIME(ms) ((ms) * -10000)
#define REPORT_ERROR(operationName) if(!NT_SUCCESS(status)){ drawError(operationName, status); return status; }

void drawError(PWSTR operationName, NTSTATUS status){
    // build message
    WCHAR message[255];
    swprintf_s(message, 255, L"%s failed: %d", operationName, status);

    // draw message
    UNICODE_STRING usMessage;
    RtlInitUnicodeString(&usMessage, message);
    NtDrawText(&usMessage);

    // wait 5 seconds
    LARGE_INTEGER timeout;
    timeout.QuadPart = MS_TO_NT_TIME(5000);
    NtDelayExecution(FALSE, &timeout);
}

// NtDelayExecution doesn't work for dynamic timeouts for some reason
void sleep(UINT32 timeout, UINT32 minTimeout){
    LARGE_INTEGER mti;
    mti.QuadPart = MS_TO_NT_TIME(minTimeout);

    LARGE_INTEGER currentTime;
    NtQuerySystemTime(&currentTime);

    UINT64 endTime = currentTime.QuadPart + (timeout * 10000);

    while(TRUE){
        NtQuerySystemTime(&currentTime);
        if(currentTime.QuadPart >= endTime) break;
        NtDelayExecution(FALSE, &mti);
    }
}

NTSTATUS NtProcessStartup(PPEB peb){
    NTSTATUS status = STATUS_SUCCESS;
    PVOID heap = NtCurrentPeb()->ProcessHeap;

    // open key
    HANDLE hKey;
    UNICODE_STRING usKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SYSTEM\\winboottext");
    OBJECT_ATTRIBUTES oaKey = RTL_CONSTANT_OBJECT_ATTRIBUTES(&usKey, OBJ_CASE_INSENSITIVE);
    status = NtOpenKey(&hKey, KEY_READ, &oaKey);
    REPORT_ERROR(L"Registry key open");

    ULONG len = 0;

    // get enabled state
    UNICODE_STRING usEnabled = RTL_CONSTANT_STRING(L"Enabled");
    NtQueryValueKey(hKey, &usEnabled, KeyValueFullInformation, NULL, 0, &len);
    REPORT_ERROR(L"query Enabled");

    PKEY_VALUE_FULL_INFORMATION enInfo = (PKEY_VALUE_FULL_INFORMATION)RtlAllocateHeap(heap, 0, len);
    NtQueryValueKey(hKey, &usEnabled, KeyValueFullInformation, (PVOID)enInfo, len, &len);
    REPORT_ERROR(L"query Enabled");

    UINT32 enabled = *(UINT32*)((PBYTE)enInfo + enInfo->DataOffset);
    RtlFreeHeap(heap, 0, (PVOID)enInfo);

    // exit if Enabled = 0
    if(!enabled){
        return STATUS_SUCCESS;
    }

    // get message count
    UNICODE_STRING usMessageCount = RTL_CONSTANT_STRING(L"MessageCount");
    NtQueryValueKey(hKey, &usMessageCount, KeyValueFullInformation, NULL, 0, &len);
    REPORT_ERROR(L"query MessageCount");

    PKEY_VALUE_FULL_INFORMATION mcInfo = (PKEY_VALUE_FULL_INFORMATION)RtlAllocateHeap(heap, 0, len);
    NtQueryValueKey(hKey, &usMessageCount, KeyValueFullInformation, (PVOID)mcInfo, len, &len);
    REPORT_ERROR(L"query MessageCount");

    UINT32 messageCount = *(UINT32*)((PBYTE)mcInfo + mcInfo->DataOffset);
    RtlFreeHeap(heap, 0, (PVOID)mcInfo);

    // process each message
    for(int i = 1; i < messageCount + 1; i++){
        // init value names
        WCHAR msgValue[25];
        swprintf_s(msgValue, 25, L"Message%d", i);

        WCHAR msgDurValue[33];
        swprintf_s(msgDurValue, 33, L"Message%dDuration", i);

        UNICODE_STRING usMsgValue;
        RtlInitUnicodeString(&usMsgValue, msgValue);

        UNICODE_STRING usMsgDurValue;
        RtlInitUnicodeString(&usMsgDurValue, msgDurValue);

        // read MessageN
        NtQueryValueKey(hKey, &usMsgValue, KeyValueFullInformation, NULL, 0, &len);
        REPORT_ERROR(L"query MessageN");

        PKEY_VALUE_FULL_INFORMATION msgInfo = (PKEY_VALUE_FULL_INFORMATION)RtlAllocateHeap(heap, 0, len);
        NtQueryValueKey(hKey, &usMsgValue, KeyValueFullInformation, (PVOID)msgInfo, len, &len);
        REPORT_ERROR(L"query MessageN");

        PWSTR message = (PWSTR)((PBYTE)msgInfo + msgInfo->DataOffset);

        // read MessageNDuration
        NtQueryValueKey(hKey, &usMsgDurValue, KeyValueFullInformation, NULL, 0, &len);
        REPORT_ERROR(L"query MessageNDuration");

        PKEY_VALUE_FULL_INFORMATION msgDurInfo = (PKEY_VALUE_FULL_INFORMATION)RtlAllocateHeap(heap, 0, len);
        NtQueryValueKey(hKey, &usMsgDurValue, KeyValueFullInformation, (PVOID)msgDurInfo, len, &len);
        REPORT_ERROR(L"query MessageNDuration");

        UINT32 duration = *(UINT32*)((PBYTE)msgDurInfo + msgDurInfo->DataOffset);

        // draw message and sleep
        UNICODE_STRING usMessage;
        RtlInitUnicodeString(&usMessage, message);
        NtDrawText(&usMessage);

        sleep(duration, MIN_TIMEOUT);

        RtlFreeHeap(heap, 0, msgInfo);
        RtlFreeHeap(heap, 0, msgDurInfo);
    }

    return STATUS_SUCCESS;
}
