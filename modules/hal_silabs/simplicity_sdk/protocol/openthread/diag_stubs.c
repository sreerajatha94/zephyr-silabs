#include <stdint.h>

#include <zephyr/sys/util.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>

bool otPlatDiagModeGet(void)
{
	return false;
}

void otPlatDiagChannelSet(uint8_t aChannel)
{
	ARG_UNUSED(aChannel);
}

void otPlatDiagRadioReceived(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
	ARG_UNUSED(aInstance);
	ARG_UNUSED(aFrame);
	ARG_UNUSED(aError);
}
