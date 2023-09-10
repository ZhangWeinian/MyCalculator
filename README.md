# 计算器


## 这是什么？
一个来自在 Cpp 编程新手的学习项目。


## 使用前的注意事项
如果自行编译源码，则编译环境需要有如下支持：
* Windows SDK 10.0.22621.0
* Qt6
* boost/multiprecision（非必要，默认不使用 boost/multiprecision 相关内容。可以通过设置 HighPrecisionOperations.h 文件中的宏定义 _use_BOOST 自行启用）
* spdlog（非必要，目前日志系统尚未完成，后续将逐步加入）

如果只执行发布的 .exe 可执行文件，则执行环境需要有如下支持：
* Qt6 相关的 .dll 文件
* Windows 必要 .dll 文件

## 出现了错误？
如果 .exe 运行时出现 “缺少动态库文件” 的提示，请考虑运行环境是否满足 *使用前的注意事项*。
如果出现了其他错误（包括使用中出现的bug），请及时反馈，感谢！
