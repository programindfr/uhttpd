#ifndef __HANDLE_H__
#define __HANDLE_H__

void hdl_peer(int peerfd);
char *hdl_peerRequest(int peerfd);
int hdl_peerTarget(const char *request);
int hdl_peerCode(int filefd);
void hdl_peerResponse(int peerfd, const char *request, int filefd, int code);

#endif/*__HANDLE_H__*/
