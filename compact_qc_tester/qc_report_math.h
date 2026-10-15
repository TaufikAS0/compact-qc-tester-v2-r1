#pragma once
#include <stdint.h>
#include <string_view>
enum class DeviceVariant { Unselected, M0, M1 };
constexpr DeviceVariant parseVariant(std::string_view s) {
  return s == "M0" ? DeviceVariant::M0 : s == "M1" ? DeviceVariant::M1 : DeviceVariant::Unselected;
}
constexpr bool pdApplicable(DeviceVariant v) { return v == DeviceVariant::M0; }
constexpr bool testApplicable(DeviceVariant v, std::string_view name) {
  return v != DeviceVariant::Unselected && !(v == DeviceVariant::M1 && name == "STUSB4500");
}
constexpr uint32_t qcCrcByte(uint32_t crc, uint8_t b) {
  crc ^= b;
  for (int i = 0; i < 8; ++i)
    crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320u : 0u);
  return crc;
}
constexpr uint32_t qcCrc(std::string_view s) {
  uint32_t crc = 0xffffffffu;
  for (char c : s)
    crc = qcCrcByte(crc, static_cast<uint8_t>(c));
  return crc ^ 0xffffffffu;
}
constexpr bool csvFormulaLead(char c) { return c == '=' || c == '+' || c == '-' || c == '@'; }
constexpr size_t qcCsvQuote(std::string_view value, char *out, size_t capacity) {
  size_t first = 0, quotes = 0;
  while (first < value.size() && static_cast<uint8_t>(value[first]) <= 32)
    ++first;
  bool protect = first < value.size() && csvFormulaLead(value[first]);
  for (char c : value)
    if (c == '"')
      ++quotes;
  size_t size = value.size() + quotes + (protect ? 1 : 0) + 2;
  if (capacity <= size)
    return 0;
  size_t at = 0;
  out[at++] = '"';
  if (protect)
    out[at++] = '\'';
  for (char c : value) {
    out[at++] = c;
    if (c == '"')
      out[at++] = '"';
  }
  out[at++] = '"';
  out[at] = 0;
  return at;
}
constexpr bool qcIsoDate(std::string_view s) {
  if (s.empty())
    return true;
  if (s.size() != 10 || s[4] != '-' || s[7] != '-')
    return false;
  for (size_t i = 0; i < 10; ++i)
    if (i != 4 && i != 7 && (s[i] < '0' || s[i] > '9'))
      return false;
  int y = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + s[3] - '0';
  int m = (s[5] - '0') * 10 + s[6] - '0', d = (s[8] - '0') * 10 + s[9] - '0';
  constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (y < 2000 || y > 2199 || m < 1 || m > 12)
    return false;
  return d >= 1 && d <= days[m - 1] + (m == 2 && y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
}
constexpr bool qcPcbNumberValid(std::string_view s){if(s.size()>64)return false;for(char c:s)if(static_cast<uint8_t>(c)<32||static_cast<uint8_t>(c)==127)return false;return true;}
constexpr bool qcWarrantyDates(std::string_view date,std::string_view production){return !date.empty()&&!production.empty()&&qcIsoDate(date)&&qcIsoDate(production)&&production<=date;}
