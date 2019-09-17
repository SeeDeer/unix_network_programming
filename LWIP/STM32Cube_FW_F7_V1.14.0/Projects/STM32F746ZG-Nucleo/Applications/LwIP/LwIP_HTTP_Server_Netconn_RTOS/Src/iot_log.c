/*****************************************************************
 * @File: iot_log.c
 * @Author: LinusZhao
 * @Date: 2019-09-17 08:12:04
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-09-17 16:59:38
 * @Description: 重写printf函数实现，可控制日志打印等级并加前缀和换行
 *****************************************************************/
#include "iot_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 添加外部向串口外设写数据的函数声明
extern int write_uart(char *buf, unsigned short nbytes);

static unsigned char Log_Level = LOG_LEVEL_ERR;

#define filename(x) strrchr(x,'\\') ? strrchr(x,'\\')+1:x

static char printbuf[512];

int set_log_level(unsigned char level)
{
    if(level > LOG_LEVEL_TRACE){
        return -1; // 参数错误
    }

    Log_Level = level;

    return 0;
}

/*****************************************************************
 * @Function: PrinfLog
 * @Description: 重新printf函数实现，可控制日志打印等级并加前缀和换行
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int PrinfLog(unsigned char level,const char *_file,unsigned short _line,const char *_func,const char *fmt,...)
{
    va_list args;
    int len = 0;

    va_start(args,fmt);
    // // printf("%s,%s[%d]",filename(__FILE__),(char *)__func__,__LINE__);

    (void)(_func); // 暂时不用这个参数

    if(level > Log_Level){
        return 0;
    }
    else if(level == LOG_LEVEL_TRACE){
        len = sprintf(printbuf,"[IoT Trace][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_DEBUG){
        len = sprintf(printbuf,"[IoT Debug][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_INFO){
        len = sprintf(printbuf,"[IoT Info][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_NOTICE){
        len = sprintf(printbuf,"[IoT Notice][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_WARN){
        len = sprintf(printbuf,"[IoT Warn][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_ERR){
        len = sprintf(printbuf,"[IoT Err][%s:%d]",filename(_file),_line);
    }

    len += vsprintf(printbuf+len, fmt, args);
    len += sprintf(printbuf+len,"\r\n");
    write_uart(printbuf,len);

    va_end(args);

    return 0;
}


/* 以下为vsprintf函数源码实现，目前还有问题，先用库里的vsprintf */
#if 0

#define ZEROPAD 1               // Pad with zero  
#define SIGN    2               // Unsigned/signed long  
#define PLUS    4               // Show plus  
#define SPACE   8               // Space if plus  
#define LEFT    16              // Left justified  
#define SPECIAL 32              // 0x  
#define LARGE   64              // Use 'ABCDEF' instead of 'abcdef'

#define is_digit(c) ((c) >= '0' && (c) <= '9')  
static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s)){
        i = i*10 + *((*s)++) - '0';
    }
    return i;
}

int iot_vsprintf(char *buf, const char *fmt, va_list args)
{
    int len,i;
    char *str, *s;
    int *ip;
    int flags; /* flags to number() */
    int field_width;    /* width of output field */
    int precision;        /* min. # of digits for integers; max number of chars for from string */
    int qualifier;        /* 'h', 'l', or 'L' for integer fields */

    //str为最终存放字符串的位置但是他随着字符串增长而增长，buf始终指向最终字符串的启始位置。fmt为格式字符串
    for (str=buf ; *fmt ; ++fmt){
        if(*fmt != '%'){
            *str++ = *fmt;
            continue;
        }

        /* process flags */
        flags = 0;
repeat:
            ++fmt; //fmt指向%的后一位
            switch(*fmt){
                case '-': flags |= LEFT; goto repeat;
                case '+': flags |= PLUS; goto repeat;
                case ' ': flags |= SPACE; goto repeat;
                case '#': flags |= SPECIAL; goto repeat;
                case '0': flags |= ZEROPAD; goto repeat;
            }
        
        /* get field width */
        field_width = -1;
        if (is_digit(*fmt)){
            field_width = skip_atoi(&fmt);
        }
        else if(*fmt == '*'){
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0){
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.'){
            ++fmt;
            if (is_digit(*fmt)){
                precision = skip_atoi(&fmt);
            }
            else if(*fmt == '*'){
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0){
                precision = 0;
            }
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L'){
            qualifier = *fmt;
            ++fmt;
        }

        switch (*fmt){
            case 'c':{
                if (!(flags & LEFT)){
                    while (--field_width > 0){
                        *str++ = ' ';
                    }
                }
                *str++ = (unsigned char) va_arg(args, int);
                while (--field_width > 0){
                    *str++ = ' ';
                }
            }break;

            default:{
                //如果格式转换符不是%，则表示出错，直接打印一个%。如果是%，那么格式转换符就是%%，就由下面if(*fmt)只输出一个%
                if (*fmt != '%'){
                    *str++ = '%';
                }
                if (*fmt){
                    *str++ = *fmt;
                }
                else
                    --fmt;
                    
                continue;
            }

        }

    }

    *str = '\0';

    return str - buf;
}
#endif
