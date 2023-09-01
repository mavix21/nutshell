#include "main.h"

void serialize_nsh_info(nsh_info_t data, unsigned char *buffer)
{
	memcpy(buffer, &data, sizeof(data));
}

void deserialize_nsh_info(unsigned char *buffer, nsh_info_t *data)
{
	memcpy(data, buffer, sizeof(*data));
}
