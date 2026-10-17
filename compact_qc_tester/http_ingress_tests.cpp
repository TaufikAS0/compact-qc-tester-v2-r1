#include "http_ingress.h"
static_assert(inspectHttpIngress("")==HttpIngress::Wait);
static_assert(inspectHttpIngress("GET /api/health HTTP/1.1\r\nHost: x")==HttpIngress::Wait);
static_assert(inspectHttpIngress("GET /api/health HTTP/1.1\r\nHost: x\r\n\r\n")==HttpIngress::Ready);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: 3\r\n\r\na")==HttpIngress::Wait);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\ncontent-length: 3\r\n\r\na=b")==HttpIngress::Ready);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: 3\r\n\r\n",4096,2)==HttpIngress::Wait);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: 3\r\n\r\n",4096,3)==HttpIngress::Ready);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: 9999\r\n\r\n")==HttpIngress::Reject);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: -1\r\n\r\n")==HttpIngress::Reject);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nContent-Length: 1\r\nContent-Length: 2\r\n\r\n")==HttpIngress::Reject);
static_assert(inspectHttpIngress("POST /api/heartbeat HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n")==HttpIngress::Reject);
static_assert(inspectHttpIngress("POST /update?nonce=123 HTTP/1.1\r\nContent-Length: 1900000\r\n\r\n")==HttpIngress::Stream);
static_assert(inspectHttpIngress("POST /api/qc/import-legacy HTTP/1.1\r\nContent-Length: 50000\r\n\r\n")==HttpIngress::Stream);
