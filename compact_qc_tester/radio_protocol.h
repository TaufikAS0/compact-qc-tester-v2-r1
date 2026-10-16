#pragma once
#include <string_view>
namespace RadioProtocol {
constexpr const char *keys[]={"CNCFG","WMCFG","TMCFG","PIDCFG","UBCFG","UPCFG","PWCFG"};
constexpr int maxima[]={63,4,1,65535,7,2,3};
constexpr bool space(char c){return c==' '||c=='\t'||c=='\r'||c=='\n';}
constexpr std::string_view trim(std::string_view s){while(!s.empty()&&space(s.front()))s.remove_prefix(1);while(!s.empty()&&space(s.back()))s.remove_suffix(1);return s;}
constexpr char upper(char c){return c>='a'&&c<='z'?c-32:c;}
constexpr bool same(std::string_view a,std::string_view b){if(a.size()!=b.size())return false;for(size_t i=0;i<a.size();++i)if(upper(a[i])!=upper(b[i]))return false;return true;}
// Only labelled values; never accept command echoes, bare OK, or arbitrary digits.
constexpr int parse(std::string_view line,int index){
 if(index<0||index>=7)return -1;line=trim(line);if(!line.empty()&&line.front()=='+')line.remove_prefix(1);
 size_t at=0;while(at<line.size()&&line[at]!=':'&&line[at]!='='&&static_cast<unsigned char>(line[at])!=0xef)++at;
 if(at==line.size())return -1;auto label=trim(line.substr(0,at));
 if(!same(label,keys[index])&&!(index==0&&same(label,"Channel"))&&!(index==1&&same(label,"Mode")))return -1;
 if(static_cast<unsigned char>(line[at])==0xef){if(line.substr(at,3)!="\xef\xbc\x9a")return -1;at+=3;}else ++at;
 auto value=trim(line.substr(at));if(value.empty()||value.size()>5)return -1;int n=0;for(char c:value){if(c<'0'||c>'9')return -1;n=n*10+c-'0';}return n<=maxima[index]?n:-1;
}
constexpr bool writeAck(std::string_view line){return trim(line)=="+OK";}
}
