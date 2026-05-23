# Muduo 网络库极简整体架构
## 第一部分
### 模块1 timestamp时间戳
``` 
timestamp时间戳 
作用：高精度的时间封装 统一系统时间获取
设计详解 继承copyable 和 type类
copyable:空基类 标记类为值类型
Types.h：定义int64_t/string等基础类型别名

```