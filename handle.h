#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "darray.h"

DA_INIT_TYPE(char, char)

void hdl_peer(int peerfd, const char *optpath);
da_array_char_t *hdl_peerRequest(int peerfd);
int hdl_peerTarget(const da_array_char_t *request, const char *optpath);
int hdl_peerCode(int filefd);
void hdl_peerResponse(int peerfd, const da_array_char_t *request, int filefd, int code);

#endif/*__HANDLE_H__*/
