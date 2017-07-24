#include <Windows.h>

#pragma comment(lib,"ntdll.lib")

void TriggerBSOD() {
	EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
	EXTERN_C NTSTATUS NTAPI NtRaiseHardError(
		IN NTSTATUS ErrorStatus,
		IN ULONG NumberOfParameters,
		IN ULONG UnicodeStringParameterMask,
		IN PULONG_PTR Parameters,
		IN ULONG ValidResponseOptions,
		OUT PULONG Response
	);
	BOOLEAN bl;
	unsigned long response;
	RtlAdjustPrivilege(19, 1, 0, &bl);
	NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, 0, 6, &response);

	return;
}