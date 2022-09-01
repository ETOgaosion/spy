# 项目架构思路

类似spy，整个项目分为若干部分组件：

- 内核模块，位于`spy/module`
- spy用户空间工具程序，位于`spy/tools`
- `.bin` firmware，位于`spy/hypervisor`
- 用户程序二进制镜像，位于`spy/target`

以上组件位置及作用如下：

- 内核模块用来做patch，向内核中cpu offline流程中probe启动函数，启动函数将：
  - 向用户空间注册设备`/dev/spy`，实现读写操作与ioctl接口
  - ioctl接口通过调用参数进行分支，前往不同的动作
- spy用户空间工具程序模仿为shell，封装了ioctl的操作，方便用户命令行执行
- 
