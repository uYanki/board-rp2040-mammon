烧录 `CircuitPython.uf2`，并将 `software` 目录下的文件全部复制到 Pico 设备中。

#### 如何运行？

方法① 使用 Thonny 运行 code.py 文件。

方法② 复位 Pico，上电后自动运行 code.py。

#### 引脚功能

上电过程中：

GPIO0 为高电平时，失能大容量存储设备。

GPIO1 为高电平时，自动运行 txt 脚本。

GPIO 2、3，4，5 是脚本选择引脚。

* GPIO 2,3 为高电平时，运行以 0011 开头的 txt 脚本。
* GPIO 3,4,5 为高电平时，运行以 1110 开头的 txt 脚本。

#### 关于 PS1 脚本

*.ps1 时 powershell 的脚本。

使用前需在 shell 中开启权限：

```sh
set-executionpolicy remotesigned
```

