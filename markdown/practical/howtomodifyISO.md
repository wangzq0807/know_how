手动修改基础版方法
一．关于本文
本文介绍如何手动修改基础版ISO。
以下以cdos.iso 代表待修改ISO镜像名字，以下以/home/cdos为工作目录。以安装cdos_1.0.0_amd64.deb 修改方式说明修改方法。
二．具体方法
1.挂载&拷贝ISO
    先挂在ISO镜像，再在工作目录创建livecd并拷贝存放ISO的内容；
#sudo  mkdir  /home/cdos/livecd
#sudo  mount  -o  loop  cdos.iso   /mnt
#sudo  rsync  -a  /mnt/  /home/cdos/livecd/
2.解压缩squashfs Image
解压缩ISO里的squashfs Image 文件，为后续修改做准备。	
#sudo  apt-get   install   squashfs-tools
#sudo  mv  /home/cdos/livecd/casper/filesystem.squashfs  /home/cdos
#sudo  unsquashfs  /home/cdos/filesystem.squashfs  #会在/home/cdos/创建squashfs-tools文件夹
3.挂载相关文件系统
#sudo  mount  -t  proc  proc  /home/cdos/squashfs-root/proc
#sudo  mount  -t  sysfs  sysfs  /home/cdos/squashfs-root/sys
#sudo  mount  -t  devtmpfs  devtmpfs  /home/cdos/squashfs-root/dev
4.需改ISO内容
在对ISO做修改前，需要先拷贝相关内容到/home/cdos/squashfs-root目录下，然后chroot后进行修改
#sudo  cp  cdos_1.0.0_amd64.deb  /home/cdos/squashfs-root/tmp/
#sudo  chroot  /home/cdos/squashfs-root/
#dpkg   -i  /tmp/cdos_1.0.0_amd64.deb
#dpkg -l | grep ii | awk '{print $2,$3}' > filesystem.manifest
5.重构squashfs  Image
#exit      #退出chroot环境
#sudo  umount  -t  /home/cdos/squashfs-root/proc
#sudo  umount  -t   /home/cdos/squashfs-root/sys
#sudo  umount  -t   /home/cdos/squashfs-root/dev
#sudo  mv  /home/cdos/squashfs-root/filesystem.manifest  /home/cdos/livecd/casper/filesystem.manifest
rm -f /home/cdos/livecd/casper/filesystem.squashfs
#sudo  mksquashfs  /home/cdos/squashfs-root   /home/cdos/livecd/casper/filesystem.squashfs
如果更新了特殊的、需要放到livecd 目录的文件（比如内核/initrd），
请不要忘记在执行下一步之前将这些文件copy到livecd 目录；
6.更新md5sum.txt
#cd /home/cdos/livecd/
#find -type f -print0 |  xargs -0 md5sum | grep -v ./md5sum.txt |  tee  md5sum.txt
7.制作ISO
在/home/cdos/下就会生成修改后的cdos-2.iso  image文件。
#sudo  apt-get  install  mkisofs
#sudo  cd  /home/cdos/livecd/
#sudo  mkisofs  -r  -V  “cdos”  -cache-inodes  -J  -l  -o  ../cdos-2.iso  -b  isolinux/isolinux.bin  -c  isolinux/boot.cat  -no-emul-boot  -boot-info-table  -boot-load-size  4  -eltorito-alt-boot  -e  boot/grub/efi.img  -no-emul-boot  .

