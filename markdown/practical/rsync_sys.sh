#! /bin/sh
srcdir=/home/wangzq/Music/src/
dstdir=/home/wangzq/Music/dst/
# --delete : 在目标目录中，删除源目录中不存在的文件
# -P : 显示进度
# -p : 保留权限
# -a : 存档模式
# -v : 
# -H : 保留硬连接
# -X : 保留扩展属性
rsync --delete -PpavAHX $srcdir $dstdir  --exclude=/media/* --exclude=/sys/* --exclude=/proc/* --exclude=/mnt/* --exclude=/tmp/* --exclude=/home/*

