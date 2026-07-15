#include <stdint.h>

#include <openthread/error.h>

#include "common/timer.hpp"

namespace ot {
namespace Utils {
namespace CmdLineParser {

// The Silicon Labs precompiled stack exports ParseAsUint32/ParseAsInt32 using
// `unsigned long`/`long` references, while the local CLI sources are compiled
// against declarations using `uint32_t`/`int32_t` (mangled here as
// `unsigned int`/`int`). Bridge the two ABIs without duplicating the full
// parser implementation.
otError ParseAsUint32(const char *aString, unsigned long &aUint32);
otError ParseAsInt32(const char *aString, long &aInt32);

otError ParseAsUint32(const char *aString, uint32_t &aUint32)
{
    unsigned long value;
    otError       error = ParseAsUint32(aString, value);

    aUint32 = static_cast<uint32_t>(value);
    return error;
}

otError ParseAsInt32(const char *aString, int32_t &aInt32)
{
    long    value;
    otError error = ParseAsInt32(aString, value);

    aInt32 = static_cast<int32_t>(value);
    return error;
}

} // namespace CmdLineParser
} // namespace Utils
} // namespace ot

extern "C" void ot_timer_milli_start_ulong(ot::TimerMilli *aTimer, unsigned long aDelay)
    asm("_ZN2ot10TimerMilli5StartEm");
extern "C" void ot_timer_milli_start_uint(ot::TimerMilli *aTimer, unsigned int aDelay)
    asm("_ZN2ot10TimerMilli5StartEj");

// The Silicon Labs blob exports TimerMilli::Start using `unsigned long`
// while locally compiled CLI objects reference the `uint32_t`/`unsigned int`
// form. Bridge the ABI mismatch without pulling in the full timer core.
extern "C" void ot_timer_milli_start_uint(ot::TimerMilli *aTimer, unsigned int aDelay)
{
    ot_timer_milli_start_ulong(aTimer, static_cast<unsigned long>(aDelay));
}
