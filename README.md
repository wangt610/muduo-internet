# Muduo 网络库极简整体架构
## 第一部分
### 模块1 timestamp时间戳
``` 
timestamp时间戳 
作用：高精度的时间封装 统一系统时间获取
设计详解 继承copyable
equality_comparable自动生成!=运算符（只需手动实现==）
less_than_comparable：自动生成>、<=、>=运算符（只需手动实现<）

注意继承的是boost里面的类 需下载源码和
```