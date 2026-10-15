#pragma once
#include <stdint.h>
namespace RtcMath {
constexpr bool leap(int y){return y%4==0&&(y%100!=0||y%400==0);}
constexpr int days(int y,int m){return m==2?(leap(y)?29:28):(m==4||m==6||m==9||m==11)?30:31;}
constexpr bool date(int y,int m,int d,int h,int n,int s){return y>=2000&&y<=2099&&m>=1&&m<=12&&d>=1&&d<=days(y,m)&&h>=0&&h<24&&n>=0&&n<60&&s>=0&&s<60;}
constexpr int dec(uint8_t b){return (b>>4)<=9&&(b&15)<=9?(b>>4)*10+(b&15):-1;}
constexpr uint8_t bcd(int n){return (n/10)*16+n%10;}
constexpr int64_t epoch(int y,int m,int d,int h,int n,int s){
 if(!date(y,m,d,h,n,s))return -1;
 int64_t total=10957;for(int i=2000;i<y;++i)total+=leap(i)?366:365;
 for(int i=1;i<m;++i)total+=days(y,i);
 return ((total+d-1)*24+h)*3600+n*60+s;
}
constexpr bool decode(const uint8_t *b,int64_t &out){
 int h=dec(b[2]&63);if(b[2]&64){int t=dec(b[2]&31);h=t<1||t>12?-1:t%12+((b[2]&32)?12:0);}
 // DS3231 supports century bit, but this contract deliberately bounds calendar to2000..2099.
 out=(b[5]&128)?-1:epoch(2000+dec(b[6]),dec(b[5]&31),dec(b[4]&63),h,dec(b[1]&127),dec(b[0]&127));
 return out>=0;
}
constexpr bool encode(int64_t e,uint8_t *b){
 if(e<946684800LL||e>4102444799LL)return false;
 int64_t day=e/86400;int rest=e%86400,y=1970;
 while(day>=(leap(y)?366:365)){day-=leap(y)?366:365;++y;}
 int m=1;while(day>=days(y,m)){day-=days(y,m);++m;}
 b[0]=bcd(rest%60);b[1]=bcd(rest/60%60);b[2]=bcd(rest/3600);b[3]=uint8_t((e/86400+4)%7+1);b[4]=bcd(day+1);b[5]=bcd(m);b[6]=bcd(y-2000);return true;
}
constexpr bool tick(int64_t start,int64_t end,int64_t elapsedMs){return elapsedMs>=60000&&elapsedMs<=180000&&end-start>=elapsedMs/1000-2&&end-start<=elapsedMs/1000+2;}
constexpr bool readback(int64_t requested,int64_t actual){return requested>=946684800LL&&requested<=4102444799LL&&actual>=requested&&actual<=requested+1&&actual<=4102444799LL;}
constexpr int64_t writeChallenge(int64_t requested,int64_t current){const int delta[]={120,-120,240,-240};for(int d:delta){int64_t candidate=requested+d;int64_t difference=candidate>current?candidate-current:current-candidate;if(candidate>=946684800LL&&candidate<=4102444799LL&&difference>2)return candidate;}return -1;}
// 0=insufficient evidence,1=PASS,-1=FAIL. Caller separately rejects non-finite/out-of-bound fields.
constexpr int evidence(double elapsed,double drift,double uncertainty,double tolerance,bool quality,bool retention,bool physical){
 if(!quality||elapsed<(retention?60:86400)||uncertainty<1||uncertainty>=tolerance||(retention&&!physical))return 0;
 double magnitude=drift<0?-drift:drift;return magnitude+uncertainty<=tolerance?1:magnitude-uncertainty>tolerance?-1:0;
}
static_assert(epoch(2000,1,1,0,0,0)==946684800LL);
static_assert(epoch(2024,2,29,0,0,0)>0&&epoch(2025,2,29,0,0,0)==-1);
static_assert(tick(100,160,60000)&&!tick(100,100,60000)&&!tick(100,160,59000));
}
