#include "Log.h"
#include <stdarg.h>
#include <ntstrsafe.h>

void LogMessage(_In_ PCSTR fmt, ...)
{
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    RtlStringCchVPrintfA(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: %s", buffer));

    UNICODE_STRING logPath;
    RtlInitUnicodeString(&logPath, L"\\??\\C:\\ProgramData\\ASIO4Krnl\\logs\\driver.log");
    OBJECT_ATTRIBUTES attrs;
    InitializeObjectAttributes(&attrs, &logPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    IO_STATUS_BLOCK iosb;
    HANDLE file;
    NTSTATUS status = ZwCreateFile(&file, FILE_APPEND_DATA | SYNCHRONIZE, &attrs,
                                   &iosb, NULL, FILE_ATTRIBUTE_NORMAL,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT,
                                   NULL, 0);
    if (NT_SUCCESS(status)) {
        SIZE_T len = strlen(buffer);
        ZwWriteFile(file, NULL, NULL, NULL, &iosb, buffer, (ULONG)len, NULL, NULL);
        ZwClose(file);
    }
}
