# TLPI-note

包含个人的练习解答,跳过了关于已弃用内容相关的题目.

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

为了获取完整的manual page(例如`man pthread_mutex_trylock`),需要安装一个包:

```
sudo apt install glibc-doc
```


TODO:

6.1

bss段的大小比所有未显式初始化的全局变量和静态变量的大小之和多出了32字节,猜测是用来记录变量位置和大小的元信息,没有验证.

7.2

malloc lab

27.2

过于trivial还很长,不想写= =

30.2

感觉就是在实现crabbing protocol,用这个来练习互斥锁大可不必...
