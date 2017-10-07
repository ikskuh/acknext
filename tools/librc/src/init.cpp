#include "librc.h"

__attribute__ ((visibility ("hidden"))) rcLogCallback librc_callback = nullptr;

C_API void librc_init(rcLogCallback callback)
{
	librc_callback = callback;
}

C_API void librc_close()
{

}
