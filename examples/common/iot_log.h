#ifndef _IOT_LOG_H
#define _IOT_LOG_H

#ifdef __cplusplus
    extern "C" {
#endif

#define LOG_LEVEL_ERR       0  // 错误信息，程序正常运行不应发生的信息 
#define LOG_LEVEL_WARN      1  // 警告信息
#define LOG_LEVEL_NOTICE    2  // 需要注意的信息
#define LOG_LEVEL_INFO      3  // 通知信息
#define LOG_LEVEL_DEBUG     4  // 程序运行调试信息，RELEASE版本中删除
#define LOG_LEVEL_TRACE     5  // 程序运行路径信息，RELEASE版本中删除

int set_log_level(unsigned char level,unsigned short buf_len);
int printf_log(unsigned char level,const char *_file,unsigned short _line,const char *_func,const char *fmt,...);

#define PRINT_LOG(level,fmt, ...) \
do \
{ \
    printf_log(level, \
             __FILE__,__LINE__,(char *)__func__, \
             fmt,##__VA_ARGS__); \
}while(0)

#define LOG_ERR(fmt, ...)    PRINT_LOG(LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)   PRINT_LOG(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...) PRINT_LOG(LOG_LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)   PRINT_LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)  PRINT_LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...)  PRINT_LOG(LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define LOG_ENTER() LOG_TRACE("enter [%s]", (char *)__func__)
#define LOG_LEAVE() LOG_TRACE("leave [%s]", (char *)__func__)


#ifdef __cplusplus
    }
#endif

#endif/* _IOT_LOG_H */

