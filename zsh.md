1.安装zsh

```shell
sudo apt-get install zsh
```

2.把默认的shell改成zsh

```shell
chsh -s /bin/zsh
```

3.配置密码文件，解决 chsh:PAM认证失败的问题

```shell
sudo vim /etc/passwd

#把第一行的/bin/bash改成/bin/zsh，这个是root用户的。
#把最后一行的/bin/bash改成/bin/zsh，这个应该是每台电脑的登录用户名+计算机名组成的
```

4.安装oh-my-zsh

```
sh -c "$(curl -fsSL https://raw.github.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"
```

5.参考链接

https://www.cnblogs.com/EasonJim/p/7863099.html