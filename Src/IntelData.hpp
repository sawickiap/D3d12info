#pragma once

// Macro set by Cmake.
#if USE_INTEL_GPUDETECT

namespace GPUDetect
{
enum
{
	INTEL_VENDOR_ID = 0x8086,
};
}

namespace IntelData
{

// Prints parameters related to Intel's library itself, regardless of selected adapter.
void PrintStaticParams();

void PrintAdapterData(IDXGIAdapter* adapter);

} // namespace Intel

#endif // #if USE_INTEL_GPUDETECT
