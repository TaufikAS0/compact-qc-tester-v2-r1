#pragma once
#include <stddef.h>
#include <string_view>

enum class HttpIngress { Wait, Ready, Stream, Reject };
constexpr bool httpKey(std::string_view a,std::string_view b) {
  if(a.size()!=b.size())return false;
  for(size_t i=0;i<a.size();++i){char c=a[i];if(c>='A'&&c<='Z')c+=32;if(c!=b[i])return false;}
  return true;
}
constexpr HttpIngress inspectHttpIngress(std::string_view data,size_t limit=4096,size_t bodyAvailable=0) {
  size_t end=data.find("\r\n\r\n");
  if(end==std::string_view::npos)return data.size()>=limit?HttpIngress::Reject:HttpIngress::Wait;
  size_t first=data.find("\r\n"),length=0;bool seen=false;
  for(size_t at=first+2;at<end;){
    size_t next=data.find("\r\n",at),colon=data.find(':',at);
    if(next==std::string_view::npos||colon==std::string_view::npos||colon>=next)return HttpIngress::Reject;
    auto key=data.substr(at,colon-at),value=data.substr(colon+1,next-colon-1);
    if(httpKey(key,"transfer-encoding"))return HttpIngress::Reject; // core has no chunked request support
    if(httpKey(key,"content-length")){
      if(seen)return HttpIngress::Reject;seen=true;
      while(!value.empty()&&(value.front()==' '||value.front()=='\t'))value.remove_prefix(1);
      while(!value.empty()&&(value.back()==' '||value.back()=='\t'))value.remove_suffix(1);
      if(value.empty())return HttpIngress::Reject;
      for(char c:value){if(c<'0'||c>'9'||length>2097152)return HttpIngress::Reject;length=length*10+c-'0';}
    }
    at=next+2;
  }
  auto line=data.substr(0,first);
  // Large deliberate uploads use the pinned streaming parser, whose callbacks
  // stop outputs/service TPL. Legacy import is a reviewed one-time large JSON.
  bool stream=line.substr(0,13)=="POST /update?" || line.substr(0,13)=="POST /update " ||
              line.substr(0,27)=="POST /api/qc/import-legacy ";
  if(stream)return HttpIngress::Stream;
  if(end+4>limit||length>limit-(end+4))return HttpIngress::Reject;
  return data.size()+bodyAvailable>=end+4+length?HttpIngress::Ready:HttpIngress::Wait;
}
