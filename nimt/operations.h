#pragma once

namespace Nimt {
	/*We declare this here becouse its an undocumented function*/
	extern "C" __declspec(dllimport)
		NTSTATUS NTAPI MmCopyVirtualMemory(
			PEPROCESS FromProcess,
			PVOID FromAddress,
			PEPROCESS ToProcess,
			PVOID ToAddress,
			SIZE_T BufferSize,
			KPROCESSOR_MODE PreviousMode,
			PSIZE_T NumberOfBytesCopied
		);

	NTSTATUS ReadMemory(_In_ INT process_id, _In_ PVOID address, _In_ PVOID buffer, _In_ SIZE_T buffer_size, _In_ SIZE_T size_copied);
	NTSTATUS WriteMemory(_In_ INT process_id, _In_ PVOID address, _In_ PVOID buffer, _In_ SIZE_T buffer_size, _In_ SIZE_T size_copied);
}
