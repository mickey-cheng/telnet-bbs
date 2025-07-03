# kbs bbs重新编译版本

一、关于本系统

大学四年，通过学校的bbs论坛认识了很多朋友，fengyuner、xuanyesun、cookie、cici、ttyy、congcon、AbitOFaLA（这货马甲多的要命什么MaNSoN、liulangzi）、taotao、xlwind、cinderella、czp、fzily、jessielee、NIC、chinaren、zdy、jwc等等，中间的很多人随着时光的流逝记忆已经很模糊了，但是那段岁月至今让人怀念。只可惜，随着高校对bbs论坛管理的加强，侏罗纪论坛从kbs bbs转为discuz，再到后面的完全关闭，过往的内容永远消失在时间的长河中。如今10多年过去了，再次回首当年的岁月，只能感叹青春再也回不来了。

某一天突发奇想，能不能自己再把bbs搭起来，去感受感受telnet登录论坛的那种畅快感。但是kbs bbs已经闭源，最后能够找到的版本已经无法再最新的linux下进行安装，需要进行修改。同时，原来的kbs bbs使用了telnet+http的方式访问，在现在国内的环境下，搞个web论坛，那基本上是天方夜谭。基于上述两个理由，使用Cursor对代码进行了修改，删除了web部分代码，对原来不兼容的内容进行了修改（修改后还未测试过）。详细安装方式见\doc\INSTALL.debian12

具体安装方案如下：

## 1. 系统准备

建议使用全新安装的 Debian 12（bookworm）系统，确保有 sudo 权限。

### 1.1 更新系统

```
sudo apt update
sudo apt upgrade
```

### 1.2 安装必要依赖

```
sudo apt install build-essential autoconf automake libtool zlib1g-dev libgmp-dev openssl exim4 bison
```

- build-essential 包含 gcc、make 等编译工具
- autoconf、automake、libtool 用于自动化构建
- zlib1g-dev、libgmp-dev、openssl 为必需库
- exim4 为系统自带邮件服务
- bison（或 byacc）用于部分转信功能

---

## 2. 创建 bbs 用户和目录

### 2.1 新建 bbs 用户和组

```
sudo adduser --home /home/bbs --shell /bin/bash bbs
```

- 按提示设置密码（如只本地测试可设简单密码）

### 2.2 创建源码目录

假设你用自己的账号操作，建议在家目录下新建源码目录：

```
mkdir -p ~/kbsbbs_src
cd ~/kbsbbs_src
```

---

## 3. 获取并解压 kbsbbs 源代码

### 3.1 下载源码

- 方式一：下载快照包（假设有 tar.gz 包）
  ```
  wget http://example.com/kbsbbs-latest.tar.gz
  tar zxvf kbsbbs-latest.tar.gz
  cd kbs_bbs
  ```
- 方式二：使用 git/svn（如有仓库地址，按实际情况操作）

### 3.2 检查站点定制文件

进入 `site/` 目录，确认有 `fb2k-v2.c` 和 `fb2k-v2.h` 文件。

#### 检查并修改 UID/GID

```
grep "^#define.*BBSUID" site/fb2k-v2.h
grep "^#define.*BBSGID" site/fb2k-v2.h
grep "^bbs:" /etc/passwd
```

- 确保 BBSUID 和 BBSGID 与 `/etc/passwd` 中 bbs 用户的 uid/gid 一致，否则编辑 fb2k-v2.h 修改为实际值。

---

## 4. 编译和安装

### 4.1 配置

```
./configure --prefix=/home/bbs --enable-site=fb2k-v2 --enable-ssh --enable-ssl
```

- --prefix=/home/bbs 指定安装目录
- --enable-site=fb2k-v2 指定站点配置
- --enable-ssh 启用 ssh 支持
- --enable-ssl 启用 pop3s 支持

### 4.2 编译

```
make
```

### 4.3 安装

```
sudo make install
sudo make install-home
sudo chown -R bbs:bbs /home/bbs
```

- make install-home 会复制数据文件和创建目录
- chown 确保 bbs 用户拥有所有权

---

## 5. 初始化 BBS

以 bbs 用户或 root 身份执行：

```
sudo -u bbs /home/bbs/bin/bootstrap
```

- 会自动注册 guest 和 SYSOP，并建立系统基本版面

---

## 6. 配置 SSH 支持（可选）

如需 ssh 登录支持，参考 doc/INSTALL.ssh，或简单配置：

```
sudo touch /home/bbs/etc/sshd_config
sudo ssh-keygen -t rsa1 -f /home/bbs/etc/ssh_host_key
sudo chown bbs:bbs /home/bbs/etc/ssh_host_key*
```

- 需安装 openssh-client（Debian 12 默认已装）

---

## 7. 启动服务

切换到 bbs 用户：

```
sudo -i -u bbs
cd /home/bbs/bin
./miscd daemon
./bbslogd
./bbsd -p 23
./sshbbsd -p 22
```

- 23端口为 telnet，22端口为 ssh
- 需 root 权限时可用 sudo 启动

---

## 8. 站点配置建议

- 站点名称、欢迎语等可在 `site/fb2k-v2.c`、`site/fb2k-v2.h` 里修改
- 系统参数可在 `site/default.h` 里调整
- bbs 用户 crontab：建议设置定时任务，详见 `doc/README.SYSOP`

---

## 9. 其他常见问题

- 共享内存不足（如遇 Bus Error）：
  ```
  sudo sysctl -w kernel.shmmax=268435456
  echo "kernel.shmmax = 268435456" | sudo tee -a /etc/sysctl.conf
  ```
- 端口被占用：确认 23/22 端口未被其他服务占用
- 邮件服务：exim4 默认即可，无需额外配置

---

## 10. 参考与求助

- 更多问题请查阅 `doc/README.SYSOP`、`doc/INSTALL.ssh` 等文档
- 也可到水木社区 (newsmth.net) BBSMan_Dev 版发帖求助
