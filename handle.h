#ifndef __HANDLE_H__
#define __HANDLE_H__

void hdl_peer(int peerfd, const char *optpath);
char *hdl_peerRequest(int peerfd);
int hdl_peerTarget(const char *request, const char *optpath);
int hdl_peerCode(int filefd);
void hdl_peerResponse(int peerfd, const char *request, int filefd, int code);

#endif/*__HANDLE_H__*/
