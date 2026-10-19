#pragma once
#include <WebServer.h>
#include <lwip/sockets.h>
#include <errno.h>
#include "http_ingress.h"
#include <utility>
#include <cstring>

// Arduino-ESP32 3.3.10 parser is synchronous. Never give it an incomplete
// header: a speculative/browser connection must not monopolize the QC loop.
// No core/library files are patched. Authentication and multipart OTA still use
// the pinned WebServer parser and existing handlers, on the main task only.
class QcWebServer : public WebServer {
  // Header bytes are consumed cooperatively across TCP segments, then replayed
  // to the stock parser. MSG_PEEK on lwIP may expose only the first segment.
  class HeaderClient : public NetworkClient {
   public:
    HeaderClient(const NetworkClient &client,String &&text):NetworkClient(client),prefix(std::move(text)){}
    int available() override {return int(prefix.length()-position)+NetworkClient::available();}
    int peek() override {return position<prefix.length()?uint8_t(prefix[position]):NetworkClient::peek();}
    int read() override {return position<prefix.length()?uint8_t(prefix[position++]):NetworkClient::read();}
    int read(uint8_t *data,size_t size) override {
      size_t n=min(size,size_t(prefix.length()-position));
      if(n){memcpy(data,prefix.c_str()+position,n);position+=n;}
      int rest=n<size?NetworkClient::read(data+n,size-n):0;
      return n==0&&rest<0?rest:int(n)+(rest>0?rest:0);
    }
   private:
    String prefix;size_t position=0;
  };
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
        p.client = incoming;p.header="";p.header.reserve(512);p.started = millis(); assigned = true; break;
      }
      if (!assigned) { incoming.stop(); ++rejectedHeaders; }
    }
    for (auto &p : pending) {
      if (p.client.fd() < 0) continue;
      // Expired requests are discarded even if they became complete while a
      // previous handler ran; never dispatch a delayed command after deadline.
      if(uint32_t(millis()-p.started)>=1500){p.client.stop();p.client=NetworkClient();++rejectedHeaders;continue;}
      while(p.header.length()<4096 && !p.header.endsWith("\r\n\r\n") && p.client.available()>0){
        int c=p.client.read();if(c<0)break;p.header+=char(c);
      }
      const auto admission=inspectHttpIngress(std::string_view(p.header.c_str(),p.header.length()),4096,p.client.available());
      if(admission==HttpIngress::Stream && (!streamingAllowed || !streamingAllowed())){
        p.client.stop();p.client=NetworkClient();++rejectedHeaders;continue;
      }
      if (admission==HttpIngress::Ready || admission==HttpIngress::Stream) {
        _currentClient = p.client; p.client = NetworkClient();
        HeaderClient buffered(_currentClient,std::move(p.header));
        _currentStatus = HC_WAIT_READ; _statusChange = millis();
        const uint32_t started = millis();
        buffered.setTimeout(5000);
        // Same parse/dispatch/finalize sequence as pinned3.3.10, no middleware
        // or SSE is configured by this firmware. Handlers remain on main task.
        if(_parseRequest(buffered)){
          _contentLength=CONTENT_LENGTH_NOT_SET;_responseCode=0;_clearResponseHeaders();
          _handleRequest();
        }
        _currentClient.stop();_currentClient=NetworkClient();_currentStatus=HC_NONE;
        _currentUpload.reset();_currentRaw.reset();
        maxHandleMs = max(maxHandleMs, uint32_t(millis()-started));
        ++completedRequests;
        return; // one handler per loop; output/watchdog checks between requests
      }
      if (!p.client.connected() ||
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
      // AP+STA shares one radio; a healthy AP receiver can pause over 1s.
      // Allow bounded backpressure while continuing OFF timers/TPL above.
      else if(uint32_t(millis()-progress)>=5000){++writeTimeouts;break;}
      delay(1); // scheduler only; safety serviced every iteration
    }
    if(sent!=length) _currentClient.stop();
    return sent;
  }
  size_t _currentClientWrite_P(PGM_P data, size_t length) override {
    return _currentClientWrite(data,length); // ESP32 flash is memory mapped
  }
 private:
  struct Pending { NetworkClient client;String header;uint32_t started=0; } pending[4];
};
