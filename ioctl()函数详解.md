##### ioctl()函数详解：

ioctl函数实现对设备的I/O通道进行管理的函数，就是对设备的一些特性进行控制，例如串口的波特率，马达的速率。函数如下：

int ioctl(int fd, int cmd,...)   其中fd就是对用户程序打开设备时使用的open函数返回的文件表示符，cmd就是用户程序对设备的控制命令，至于后面的省略是补充的数据，一般最多一个，这个参数有无与cmd的意义有关系。

##### ioctl()的必要性：

如果不使用ioctl()的话，也可以实现对设备I/O通道的控制，但是比较蛮拧。例如驱动程序中实现write()的时候检查一下是否有特殊的数据流通过，如果有的话，那么后面就跟着控制命令（一般socket编程中常常这样做），但是如果是这样的话，会导致代码分工不明确，程序结构混乱，所以使用ioctl（）函数来实现控制的功能。用户只是通过命令码cmd告诉驱动想要做什么。至于这些命令如何实现和怎么实现这些命令，都是驱动程序要做的。

##### ioctl()如何实现 

在驱动程序中实现的ioctl函数体内，实际上是有一个switch{case}结构，每一个case对应一个命令码，做出一些相应的操作。怎么实现这些操作，这是每一个程序员自己的事情。因为设备都是特定的，这里也没法说。关键在于怎样组织命令码，因为在ioctl中命令码是唯一联系用户程序命令和驱动程序支持的途径。命令码的组织是有一些讲究的，因为我们一定要做到命令和设备是一一对应的，这样才不会将正确的命令发给错误的设备，或者是把错误的命令发给正确的设备，或者是把错误的命令发给错误的设备。这些错误都会导致不可预料的事情发生，而当程序员发现了这些奇怪的事情的时候，再来调试程序查找错误，那将是非常困难的事情。



**Linux系统ioctl使用示例**
These were writed and collected by kf701,
you can use and modify them but NO WARRANTY.
Contact with me : 
程序1：检测接口的inet_addr, netmask, broad_addr
程序2：检查接口的物理连接是否正常
程序3：测试物理连接
程序4：调节音量

***************************程序1****************************************
\#include <stdio.h>
\#include <string.h>
\#include <stdlib.h>
\#include <errno.h>
\#include <unistd.h>
\#include <sys/types.h>
\#include <sys/socket.h>
\#include <netinet/in.h>
\#include <arpa/inet.h>
\#include <sys/ioctl.h>
\#include <net/if.h>

static void usage()
{
  printf("usage : ipconfig interface \n");
  exit(0);
}

int main(int argc,char **argv)
{
  struct sockaddr_in *addr;
  struct ifreq ifr;
  char *name,*address;
  int sockfd;

  if(argc != 2) usage();
  else name = argv[1];

  sockfd = socket(AF_INET,SOCK_DGRAM,0);
  strncpy(ifr.ifr_name,name,IFNAMSIZ-1);

  if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
   perror("ioctl error"),exit(1);

  addr = (struct sockaddr_in *)&(ifr.ifr_addr);
  address = inet_ntoa(addr->sin_addr);
  printf("inet addr: %s ",address);

  if(ioctl(sockfd,SIOCGIFBRDADDR,&ifr) == -1)
   perror("ioctl error"),exit(1);

  addr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
  address = inet_ntoa(addr->sin_addr);
  printf("broad addr: %s ",address);

  if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) == -1)
   perror("ioctl error"),exit(1);
  addr = (struct sockaddr_in *)&ifr.ifr_addr;
  address = inet_ntoa(addr->sin_addr);
  printf("inet mask: %s ",address);

  printf("\n");
  exit(0);
}

******************************** 程序2*****************************************************
\#include <stdio.h>
\#include <string.h>
\#include <errno.h>
\#include <fcntl.h>
\#include <getopt.h>
\#include <sys/socket.h>
\#include <sys/ioctl.h>
\#include <net/if.h>
\#include <stdlib.h>
\#include <unistd.h>
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;
\#include <linux/ethtool.h>
\#include <linux/sockios.h>

int detect_mii(int skfd, char *ifname)
{
  struct ifreq ifr;
  u16 *data, mii_val;
  unsigned phy_id;

  /* Get the vitals from the interface. */
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

  if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
   {
     fprintf(stderr, "SIOCGMIIPHY on %s failed: %s\n", ifname, strerror(errno));
     (void) close(skfd);
     return 2;
   }

  data = (u16 *)(&ifr.ifr_data);
  phy_id = data[0];
  data[1] = 1;

  if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0)
   {
    fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
    return 2;
   }

  mii_val = data[3];
  return(((mii_val & 0x0016) == 0x0004) ? 0 : 1);
}

int detect_ethtool(int skfd, char *ifname)
{
  struct ifreq ifr;
  struct ethtool_value edata;
  memset(&ifr, 0, sizeof(ifr));
  edata.cmd = ETHTOOL_GLINK;

  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
  ifr.ifr_data = (char *) &edata;

  if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1)
   {
    printf("ETHTOOL_GLINK failed: %s\n", strerror(errno));
    return 2;
   }

  return (edata.data ? 0 : 1);
}

int main(int argc, char **argv)
{
  int skfd = -1;
  char *ifname;
  int retval;

  if( argv[1] ) ifname = argv[1];
   else ifname = "eth0";

  /* Open a socket. */
  if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
   {
     printf("socket error\n");
     exit(-1);
   }

  retval = detect_ethtool(skfd, ifname);
  if (retval == 2)
   retval = detect_mii(skfd, ifname);

  close(skfd);
 
  if (retval == 2)
   printf("Could not determine status\n");
  if (retval == 1)
   printf("Link down\n");
  if (retval == 0)
   printf("Link up\n");

  return retval;
}

*******************************程序3*****************************************************
\#include <stdio.h>
\#include <stdlib.h>
\#include <string.h>
\#include <errno.h>
\#include <net/if.h>
\#include <linux/sockios.h>
\#include <sys/ioctl.h>

\#define LINKTEST_GLINK 0x0000000a

struct linktest_value {
    unsigned int  cmd;
    unsigned int  data;
};

static void usage(const char * pname)
{
  fprintf(stderr, "usage: %s <device>\n", pname);
  fprintf(stderr, "returns: \n");
  fprintf(stderr, "\t 0: link detected\n");
  fprintf(stderr, "\t%d: %s\n", ENODEV, strerror(ENODEV));
  fprintf(stderr, "\t%d: %s\n", ENONET, strerror(ENONET));
  fprintf(stderr, "\t%d: %s\n", EOPNOTSUPP, strerror(EOPNOTSUPP));
  exit(EXIT_FAILURE);
}

static int linktest(const char * devname)
{
  struct ifreq ifr;
  struct linktest_value edata;
  int fd;

  /* setup our control structures. */
  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, devname);

  /* open control socket. */
  fd=socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0 ) 
   return -ECOMM;

  errno=0;
  edata.cmd = LINKTEST_GLINK;
  ifr.ifr_data = (caddr_t)&edata;

  if(!ioctl(fd, SIOCETHTOOL, &ifr)) 
   {
    if(edata.data) 
     {
      fprintf(stdout, "link detected on %s\n", devname);
      return 0;
     } else 
       {
        errno=ENONET;
       }
   }

  perror("linktest");
  return errno;
}

int main(int argc, char *argv[])
{
  if(argc != 2) 
   {
     usage(argv[0]);
   }
  return linktest(argv[1]);
}

*************************************程序4*********************************************************
\#include <sys/types.h>
\#include <sys/stat.h>
\#include <fcntl.h>
\#include <sys/ioctl.h>
\#include <sys/soundcard.h>
\#include <stdio.h>
\#include <unistd.h>
\#include <math.h>
\#include <string.h>
\#include <stdlib.h>
\#define BASE_VALUE 257

int main(int argc,char *argv[])
{
  int mixer_fd=0;
  char *names[SOUND_MIXER_NRDEVICES]=SOUND_DEVICE_LABELS;
  int value,i;

  printf("\nusage:%s dev_no.[0..24] value[0..100]\n\n",argv[0]);
  printf("eg. %s 0 100\n",argv[0]);
  printf("will change the volume to MAX volume.\n\n");
  printf("The dev_no. are as below:\n");

  for (i=0;i<SOUND_MIXER_NRDEVICES;i++)
   {
    if (i%3==0) printf("\n");
    printf("%s:%d\t\t",names[i],i);
   }

  printf("\n\n");

  if (argc<3) exit(1);

  if ((mixer_fd = open("/dev/mixer",O_RDWR)))
   {
     printf("Mixer opened successfully,working...\n");
     value=BASE_VALUE*atoi(argv[2]);

​     if (ioctl(mixer_fd,MIXER_WRITE(atoi(argv[1])),&value)==0)
​      printf("successfully.....");
​     else
​      printf("unsuccessfully.....");
​    
​     printf("done.\n");
   }
  else
   printf("can't open /dev/mixer error....\n");

exit(0);

}