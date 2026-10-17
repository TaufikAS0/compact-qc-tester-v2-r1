#pragma once
#include <WebServer.h>
#include <lwip/sockets.h>
#include <errno.h>
#include "http_ingress.h"

// Arduino-ESP32 3.3.10 parser is synchronous. Never give it an incomplete
// header: a speculative/browser connection must not monopolize the QC loop.
// No core/library files are patched. Authentication and multipart OTA still use
// the pinned WebServer parser and existing handlers, on the main task only.
class QcWebServer : public WebServer {
 public:
  explicit QcWebServer(int port) : WebServer(port) {}
  void (*serviceSafety)() = nullptr;
  bool (*streamingAllowed)() = nullptr;
  uint32_t rejectedHeaders = 0, completedRequests = 0, maxHandleMs = 0;
  uint32_t writeTimeouts = 0;
  void handleClient() {
    // Four bounded slots allow a real request past idle/partial connections.
    NetworkClient incoming = _server.accept();
    if (incoming) {
      bool assigned = false;
      for (auto &p : pending) if (p.client.fd() < 0) {
        p.client = incoming; p.started = millis(); assigned = true; break;
      }
      if (!assigned) { incoming.stop(); ++rejectedHeaders; }
    }
    for (auto &p : pending) {
      if (p.client.fd() < 0) continue;
      // Expired requests are discarded even if they became complete while a
      // previous handler ran; never dispatch a delayed command after deadline.
      if(uint32_t(millis()-p.started)>=1500){p.client.stop();p.client=NetworkClient();++rejectedHeaders;continue;}
      // MSG_PEEK does not consume bytes or populate NetworkClient's RX buffer.
      const int n = recv(p.client.fd(), header, sizeof(header), MSG_PEEK | MSG_DONTWAIT);
      const auto admission=n>0?inspectHttpIngress(std::string_view(header,n)):HttpIngress::Wait;
      if(admission==HttpIngress::Stream && (!streamingAllowed || !streamingAllowed())){
        p.client.stop();p.client=NetworkClient();++rejectedHeaders;continue;
      }
      if (admission==HttpIngress::Ready || admission==HttpIngress::Stream) {
        _currentClient = p.client; p.client = NetworkClient();
        _currentStatus = HC_WAIT_READ; _statusChange = millis();
        const uint32_t started = millis();
        WebServer::handleClient();
        maxHandleMs = max(maxHandleMs, uint32_t(millis()-started));
        ++completedRequests;
        return; // one handler per loop; output/watchdog checks between requests
      }
      if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) ||
          admission==HttpIngress::Reject || uint32_t(millis()-p.started) >= 1500) {
        p.client.stop(); p.client = NetworkClient(); ++rejectedHeaders;
      }
    }
  }
 protected:
  // Core NetworkClient::write can retry for seconds while the receiver is
  // stalled. Service OFF timers/TPL during backpressure and bound inactivity.
  size_t _currentClientWrite(const char *data, size_t length) override {
    size_t sent=0; uint32_t progress=millis();
    while (sent < length) {
      if (serviceSafety) serviceSafety();
      int n=::send(_currentClient.fd(),data+sent,min(size_t(1460),length-sent),MSG_DONTWAIT);
      if(n>0){sent+=n;progress=millis();}
      else if(n<0 && errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINTR) break;
      else if(uint32_t(millis()-progress)>=1000){++writeTimeouts;break;}
      delay(1); // scheduler only; safety serviced every iteration
    }
    if(sent!=length) _currentClient.stop();
    return sent;
  }
  size_t _currentClientWrite_P(PGM_P data, size_t length) override {
    return _currentClientWrite(data,length); // ESP32 flash is memory mapped
  }
 private:
  struct Pending { NetworkClient client; uint32_t started=0; } pending[4];
  char header[4096];
};
