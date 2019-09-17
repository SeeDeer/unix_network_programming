/*
 * @File: Do not edit
 * @Author: LinusZhao
 * @Date: 2019-09-17 15:43:43
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-09-17 16:59:19
 * @Description: Do not edit
 */

#ifndef __IOT_LOG_H
#define __IOT_LOG_H

#ifdef __cplusplus
    extern "C" {
#endif

extern int set_log_level(unsigned char level);
extern int PrinfLog(unsigned char level,const char *_file,unsigned short _line,const char *_func,const char *fmt,...);

#define SAK_PRINT_LOG(level,fmt, ...) \
do \
{ \
    PrinfLog(level, \
             __FILE__,__LINE__,(char *)__func__, \
             fmt,##__VA_ARGS__); \
}while(0)

// typedef INT_T LOG_LEVEL;
#define LOG_LEVEL_ERR       0  // 错误信息，程序正常运行不应发生的信息 
#define LOG_LEVEL_WARN      1  // 警告信息
#define LOG_LEVEL_NOTICE    2  // 需要注意的信息
#define LOG_LEVEL_INFO      3  // 通知信息
#define LOG_LEVEL_DEBUG     4  // 程序运行调试信息，RELEASE版本中删除
#define LOG_LEVEL_TRACE     5  // 程序运行路径信息，RELEASE版本中删除

#ifdef DEBUG
#define PR_DEBUG
#else

#endif

#define PR_ERR(fmt, ...)    SAK_PRINT_LOG(LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define PR_WARN(fmt, ...)   SAK_PRINT_LOG(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define PR_NOTICE(fmt, ...) SAK_PRINT_LOG(LOG_LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#define PR_INFO(fmt, ...)   SAK_PRINT_LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define PR_DEBUG(fmt, ...)  SAK_PRINT_LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define PR_TRACE(fmt, ...)  SAK_PRINT_LOG(LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define PR_TRACE_ENTER() PR_TRACE("enter [%s]", (char *)__func__)
#define PR_TRACE_LEAVE() PR_TRACE("leave [%s]", (char *)__func__)


#ifdef __cplusplus
    }
#endif


#endif /*__IOT_LOG_H*/
