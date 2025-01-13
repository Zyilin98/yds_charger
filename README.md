## 简介
本项目 是一个基于 [liaozhelin/yds-charger](https://github.com/liaozhelin/yds-charger) 和 [ktx888/yds_charger](https://github.com/ktx888/yds_charger) 的项目。
当前的源码修改自 [ktx888/yds_charger](https://github.com/ktx888/yds_charger)。
感谢两位前辈以及QQ交流群(613826801)的群友。

## 目的

本项目的当前目的是修复一部分软件问题，使设备能够实现基本的功能。
目前主要修复天气api
之后可能会修复一些功能，顺带填上前人挖的坑

并希望有大佬接手以后的开发工作

#### Tips
如果在刷入1.6.6版本之后遇到天气故障的情况。请重刷固件，在刷固件之前先使用工具eraser，重新刷写1.6.6固件。
随后配网后在上位机内的所在城市留空或者填写市一级的地址，如果不填写城市，api取你的ip地址所在地的天气。天气api故障较多，可能以后不接天气api的故障修复
新的天气api移植正在绝赞进行中

#### Develop
目前Dev分支正在进行适配esp-idf5.0.7的工作
