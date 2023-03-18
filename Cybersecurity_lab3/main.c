#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>

int main(int argc, char *argv[])
{
    printf("程序已启动,开始自定义创建网络数据包\n");
    while(1){
        char *src_ip_str = (char*)malloc(17);
        char *dst_ip_str = (char*)malloc(17);
        int sp,dp;
        printf("请输入数据包的源ip地址:");
        scanf("%s",src_ip_str);
        printf("请输入数据包的目的ip地址:");
        scanf("%s",dst_ip_str);
        printf("请输入数据包的源端口号:");
        scanf("%d",&sp);
        printf("请输入数据包的目的端口号:");
        scanf("%d",&dp);
        char send_msg[1000] = "";
        char err_buf[100] = "";
        libnet_t *lib_net = NULL;
        int lens = 0;
        libnet_ptag_t lib_t = 0;
        unsigned char src_mac[6] = {0x00, 0x0c, 0x29, 0x1b, 0x22, 0x0a};    //发送者网卡地址
        unsigned char dst_mac[6] = {0x00, 0x0c, 0x29, 0xca, 0x35, 0xc9};    //接收者网卡地址
        unsigned long src_ip, dst_ip = 0;
        int res = 0;
        printf("请输入数据包的负载数据:");
        char* content = (char*)malloc(1000);
        scanf("%s",content);
        lens = sprintf(send_msg,"%s", content);//得到send_msg的长度,负载长度
        lib_net = libnet_init(LIBNET_LINK_ADV, NULL, err_buf);	//初始化
        if(NULL == lib_net){
            perror("libnet_init");
            exit(-1);
        }
        //将字符串转换为网络字节序
        src_ip = libnet_name2addr4(lib_net, src_ip_str, LIBNET_RESOLVE);
        dst_ip = libnet_name2addr4(lib_net, dst_ip_str, LIBNET_RESOLVE);
        //构造udp数据包
        lib_t = libnet_build_udp(sp,
                                 dp,
                                 8 + lens,
                                 0,
                                 send_msg,
                                 lens,
                                 lib_net,
                                 0);
        //构造ip数据包
        lib_t = libnet_build_ipv4(20+8+lens,
                                  0,
                                  500,
                                  0,
                                  10,
                                  17,
                                  0,
                                  src_ip,
                                  dst_ip,
                                  NULL,
                                  0,
                                  lib_net,
                                  0);
        //构造以太网数据包
        lib_t = libnet_build_ethernet((u_int8_t *)dst_mac,
                                      (u_int8_t *)src_mac,
                                      ETHERTYPE_IP,
                                      NULL,
                                      0,
                                      lib_net,
                                      0);

        res = libnet_write(lib_net);	//发送数据包
        if(-1 == res){
            perror("libnet_write");
            exit(-1);
        }
        libnet_destroy(lib_net);	//销毁资源
        printf("----ok-----\n");
    }
    return 0;
}