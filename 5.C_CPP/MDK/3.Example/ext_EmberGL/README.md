基于 `Pico_Template-2.20` 进行修改：

① 重定向 `pico-sdk` 目录

② 移除原项目的 bsp、cmsis-dsp、systick_wrapper_ual.s

![1](.assest/README/1.png)

③ 宏定义

![2](.assest/README/2.png)

注：这里的 app 一次最多只能添加一个，多了会导致程序无法运行。

![3](.assest/README/3.png)