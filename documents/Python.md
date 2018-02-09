# Python
## 调试
### pdb调试
>`python -m pdb program`
## GLib
### 自定义Signal
```python
from gi.repository import GObject

class PyGObject( Gobject.Gobject )
    __gsignals__ = { 'signal' : (GObject.SIGNAL_RUN_FIRST, None，
                        (GObject.TYPE_STRING,) ) }
    def gen_signal(self):
        self.emit('signal', 'custom signal')
```
### 主线程空闲时执行
有些操作不能在sub loop中执行，必须要将其加入main loop。例如, 通过DBUS和AptClient来安装软件的操作。
> GLib.idle_add(func, param...)
## 时间
```python
import datetime
# 当前时间
now = datetime.datetime.now()
# 格式化时间
now.strftime('%Y:%m:%d:%H:%M:%S')
# 时间间隔
delta = datetime.timedelta(days=0, seconds=0, microseconds=0, milliseconds=0, minutes=0, hours=0, weeks=0)
# 时间间隔总秒数（带符号）
delta.total_seconds()
# 时间运算
# 1. timedelta +/- timedelta = timedelta
# 2. datetime  +/- timedelta = datetime
# 3. datetime  +/- datetime = timedelta
# 时间比较

```

## DBUS 进程间通信
### System Bus
#### 配置文件
1. DBUS: `/etc/dbus-1/system.d/`
1. 服务: `/usr/share/dbus-1/system-services/`

### Session Bus
## sqlite3
## subprocess
## urllib
## threading
## socket