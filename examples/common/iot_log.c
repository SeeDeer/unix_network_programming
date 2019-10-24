/*****************************************************************
 * @File: common/iot_log.c
 * @Author: LinusZhao
 * @Date: 2019-10-09 20:04:02
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-24 21:34:07
 * @Description: Debug日志管理
 *              iot_hal_uart_send函数请实现
 *****************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "iot_log.h"

static int __write_uart(int fd,const void *buf,unsigned short nbytes);
static unsigned char gst_log_level = LOG_LEVEL_DEBUG;
static unsigned short gst_log_buf_len = 512;

#define     OPERATING_SYSTEM        SYS_LINUX
// 根据文件绝对路径获取文件名称
#if OPERATING_SYSTEM == SYS_LINUX
        #define filename(x) strrchr(x,'/') ? strrchr(x,'/')+1:x
#else  /* windows*/
        #define filename(x) strrchr(x,'\\') ? strrchr(x,'\\')+1:x
#endif

/*****************************************************************
 * @Function: set_log_level
 * @Description: 设置日志输出等级 & 缓存大小
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int set_log_level(unsigned char level,unsigned short buf_len)
{
        if(level > LOG_LEVEL_TRACE){
                return -1; // 参数错误
        }

        gst_log_level = level;
        gst_log_buf_len = buf_len;

        return 0;  
}

/*****************************************************************
 * @Function: printf_log
 * @Description: 重新printf函数实现，可控制日志打印等级并加前缀和换行
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int printf_log(unsigned char level,const char *_file,unsigned short _line,const char *_func,const char *fmt,...)
{
    va_list args;
    int len = 0;

    va_start(args,fmt);

    (void)(_func); // 暂时不用这个参数

    char *printbuf = (char *)malloc(gst_log_buf_len*sizeof(char));
    if(printbuf == NULL){
        return -1;
    }

    if(level > gst_log_level){
        return -1; // 日志等级错误
    }
    else if(level == LOG_LEVEL_TRACE){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Trace][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_DEBUG){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Debug][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_INFO){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Info][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_NOTICE){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Notice][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_WARN){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Warn][%s:%d]",filename(_file),_line);
    }
    else if(level == LOG_LEVEL_ERR){
        len = snprintf(printbuf,gst_log_buf_len,"[IoT Err][%s:%d]",filename(_file),_line);
    }

    len += vsprintf(printbuf+len, fmt, args);
    len += sprintf(printbuf+len,"\r\n");
    __write_uart(STDOUT_FILENO,printbuf,len);

    va_end(args);

    free(printbuf);

    return 0;
}

/*****************************************************************
 * @Function: __write_uart
 * @Description: Do not edit
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
static int __write_uart(int fd,const void *buf,unsigned short nbytes)
{
        int ret = 0;
#if OPERATING_SYSTEM == SYS_LINUX
        ret = write(fd,buf,(size_t)nbytes);
#else
        u16_t i = 0;
        char_t *p_buf = (char_t *)buf;
        for ( i = 0; i < nbytes; i++){
                iot_hal_uart_send(p_buf[i]);
        }
        ret = i;
#endif

        return ret;
}


/* 验证 */
#if 0
int main(void)
{
        set_log_level(LOG_LEVEL_TRACE,512);
        LOG_ENTER();
        LOG_NOTICE("LOG_LEVEL:%d buf_len:%d",LOG_LEVEL_DEBUG,512);
        LOG_DEBUG("is debug");
        LOG_LEAVE();

        return 0;
}

#endif


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
