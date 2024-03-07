#include "../includes/HTTP.hpp"

void HTTP::ResetMethod()
{
    response_ready = false;
    BodyLength = 0;
    HeaderState = 0;
    Path = "";
    Version = "";
    Request_header.clear();
    Response.clear();
    boundary = "";
    Uri = "";
    htype = 0;
    ChunkHexStr = "";
    HexaChunkStatus = 0;
    ChunkSize = 0;
    PostFd = 0;
    AmountRecv = 0;
    PostPath = "";
    MimeType = "";
    requested_file_fd = -1;
    is_header_sent = false;
}