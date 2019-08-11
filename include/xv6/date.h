#if !defined(__INCLUDE_XV6_DATE_H)
#define __INCLUDE_XV6_DATE_H
#if defined(__cplusplus)
extern "C"
{
#endif
struct rtcdate {
  uint second;
  uint minute;
  uint hour;
  uint day;
  uint month;
  uint year;
};
#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_DATE_H
