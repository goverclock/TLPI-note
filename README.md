# TLPI-note

包含个人的练习解答,跳过了关于已弃用内容相关的题目.

**官方勘误表**

https://man7.org/tlpi/errata/index.html

**使用的环境**

3~33章:

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

由于服务器租期到了,34章及之后用的是自己的硬件:

```
$ cat /etc/os-release 
PRETTY_NAME="Debian GNU/Linux 11 (bullseye)"
NAME="Debian GNU/Linux"
VERSION_ID="11"
VERSION="11 (bullseye)"
VERSION_CODENAME=bullseye
ID=debian
...
```


tip:为了获取完整的manual page(例如`man pthread_mutex_trylock`),需要安装一个包:

```
sudo apt install glibc-doc
sudo apt install manpages-dev
```
