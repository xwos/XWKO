# 介绍

玄武内核模块（简称XWKO）是**XWOS**[[@gitee](https://gitee.com/xwos/XWOS)/[@github](https://github.com/xwos/XWOS)]在Linux内核中实现的一个兼容层。
通过XWKO，用户可以在Linux内核中使用XWOS的APIs。
XWKO支持的Linux内核版本范围：**3.0.x ~ 5.x.x**。


# 源码结构

-   编译
    -   xwbs: 编译系统
-   BSP与配置
    -   xwcd: 架构、CPU、SOC、外设驱动相关的代码
    -   xwbd: 工程配置、工程驱动与工程应用程序
-   操作系统内核
    -   xwos: 内核
-   中间件
    -   xwmd: 中间件
-   应用
    -   xwam: 示例代码、应用代码


# 开源协议

玄武内核模块以**Dual MPL/GPL**的形式发布。


# 文档

+   玄武内核模块快速上手指南
    -   <http://xwos.tech/QuickGuide/xwko/01-XuanWuKO-Guide/>
    -   <http://xwos.org/QuickGuide/xwko/01-XuanWuKO-Guide/>
