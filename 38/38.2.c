/* 编写一个与 sudo(8)程序类似的 set-user-ID-root 程序。这个程序应该像下面这样接
收命令行选项和参数：
$ ./douser [-u user] program-file arg1 arg2 ...
 douser 程序使用给定的参数执行 program-file，就像是被 user 运行一样。（如果省略
了–u 选项，那么 user 默认为 root。）在执行 program-file 之前，douser 应该请求 user
的密码并将密码与标准密码文件进行比较（参见程序清单 8-2），接着将进程的用户
和组 ID 设置为与该用户对应的值。 */

