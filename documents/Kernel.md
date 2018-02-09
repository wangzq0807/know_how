## 内核升级
1. 记下`s3g`,`zx`驱动的版本号，然后用`dpkg -P`删除
1. `dpkg --list`查看内核安装包
1. `dpkg -i xxx`安装新的内核包(只安装上一步列出的)
1. 删除旧的内核(如果有的话)
1. 安装`s3g`,`zx`驱动
1. 拷贝`boot/vmlinux, boot/initrd` 到 `casper/xxx, casper/initrd.lz`

## 内核参数
### 输出到串口
`/boot/grub/grub.cfg`中加入
* console=ttyS0, 115200n8 loglevel=7 log_buf_len=10M
## 其他
* gpu log `/var/log/gpu.log`
* fastboot `/boot/grub/grub.cfg` 注掉linux行
* 查看显卡
```sh
lspci -n
```