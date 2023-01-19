# TLPI-note

官方勘误表:

https://man7.org/tlpi/errata/index.html

使用的环境:

```
ubuntu@VM-4-12-ubuntu:~$ cat /etc/os-release
NAME="Ubuntu"
VERSION="20.04.4 LTS (Focal Fossa)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 20.04.4 LTS"
VERSION_ID="20.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=focal
UBUNTU_CODENAME=focal
```

TODO:

5.7

这里有一个疑问,为了保证原子性只能调用一次read/write,这种情况下如果fd是一个套接字,终端之类的文件,就可能无法读完数据.

这种情况下返回值会低于iov的总长度,缓冲区里尚未发送的数据/填充的空间怎么处理呢?

6.1

bss段的大小比所有未显式初始化的全局变量和静态变量的大小之和多出了32字节,猜测是用来记录变量位置和大小的元信息,没有验证.
