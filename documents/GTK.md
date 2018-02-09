## 国际化
* 生成翻译文件, 以`_()`标记待翻译的文本:
> xgettext -k_ -o internation.po internation.c
* 编译翻译文件:
> msgfmt -o internation.mo internation.po
* 移动mo文件到/usr/share/locale/zh_CN/xxxx
* 代码中调用:
```python
import gettext
gettext.bindtextdomain('internation', "/usr/share/locale/")
_ = gettext.gettext
# 待翻译的文本
_('some text')
```