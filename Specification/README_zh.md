# 日志规范
前言

良好的日志书写使用将为开发和运维提供便利；良好的日志级别定义和配置，将在为快速定位问题的前提上，可以有效地提升性能。

根据约束力强弱及故障敏感性,规约依次分为<font color=red>［强制］</font>、<font color=green>［推荐］</font>、<font color=blue>［参考］</font>三大类。

对于规约条目的延伸信息中,&quot;说明&quot;对内容做了引申和解释;

&quot;<font color=green>［正例］</font>&quot;提倡什么样的编码和实现方式;

&quot;<font color=red>［反例］</font>&quot;说明需要提防的雷区,以及真实的错误案例。

# 工具组件选用

## PHP

<font color=green>［推荐］</font>使用SeasLog作为日志记录工具。

较Log4php,MonoLog而言，具有更好的性能和使用便捷性。

<font color=blue>［参考］</font> [http://pecl.php.net/package/seaslog](http://pecl.php.net/package/seaslog)

## JAVA

<font color=green>［推荐］</font>使用log4j2作为日志记录工具。

较log4j,logback而言，log4j2的异步模式具有更好的性能。

<font color=blue>［参考］</font>[http://logging.apache.org/log4j/2.x/manual/async.html#AllAsync](http://logging.apache.org/log4j/2.x/manual/async.html#AllAsync)

# 目录名称

## 根目录

<font color=red>［强制］</font>日志根目录通过日志组件配置决定。

<font color=red>［强制］</font>不允许将日志根目录写死位置。

## 日志目录

<font color=red>［强制］</font>通过配置或程序中定义Logger指定日志分级目录。

<font color=green>［推荐］</font>每一个Logger代表一个具体的应用逻辑层，为每一个Logger，在根目录中分配一个日志分目录。

<font color=green>［正例］</font>根目录  /data/logs/tsb/

用户逻辑分级目录 /data/logs/tsb/user

移动逻辑分级目录 /data/logs/tsb/mobile

# 级别定义

## 定义

相较于PHP与Java各自规范不同，我们统一为8个级别，其中包括强制级别和推荐级别。

<font color=red>［强制］</font>DEBUG

debug信息、细粒度信息事件

如：调试信息

<font color=red>［强制］</font>INFO

重要事件、强调应用程序的运行过程

如：用户登录的SQL信息、创建任务时的执行过程

<font color=green>［推荐］</font>NOTICE

一般重要性事件、执行过程中较INFO级别更为重要的信息

如：调用外部API时的过程日志

<font color=red>［强制］</font>WARNING / WARN

出现了非错误性的异常信息、潜在异常信息、需要关注并且需要修复

如：调用了已经被弃用的API、用户请求参数中包含了非法字符但经过处理无害

<font color=red>［强制］</font>ERROR

运行时出现的错误、不必要立即进行修复、不影响整个逻辑的运行、需要记录并做检测

如：调用预期存在的Cache出现未命中进而查询DB、调用某首选API不通进而调用候选API

<font color=red>［强制］</font>CRITICAL  /  FATAL

紧急情况、需要立刻进行修复、程序组件不可用

如：程序组件异常退出、用户注册逻辑不能发送邮件

<font color=green>［推荐］</font>ALERT

必级立即采取行动的紧急事件、需要立即通知相关人员紧急修复

如：整个网站垮掉、DB/Cache无法连接

<font color=green>［推荐］</font>EMERGENCY

系统不可用

如：磁盘不可写

## 配置

<font color=red>［强制］</font>通过配置决定输出某级别以上的日志信息。

<font color=green>［正例］</font>在php.ini或SeasLog.ini中设置seaslog.level值，控制只输出日志级别INFO以上级别的信息。

<font color=blue>［参考］</font> [SeasLog 配置](https://github.com/SeasX/SeasLog/blob/master/README_zh.md#seaslogini%E7%9A%84%E9%85%8D%E7%BD%AE)

<font color=green>［正例］</font>在 log4j.configurationFile 指定的配置文件中，为log4j2设置Logger Level，控制只输出日志级别INFO以上级别的信息。

<font color=blue>［参考］</font> [Log4j2 配置](http://logging.apache.org/log4j/2.x/manual/configuration.html#Loggers)

# 日志文件

## 文件命名

<font color=red>［强制］</font>以 &quot; {日期} {文件名分隔符} [{级别}] . log &quot; 格式命名

<font color=red>［强制］</font>{日期}格式可选范围：yyyymmdd (年月日) , yyyymmddhh (年月日时)

<font color=green>［推荐］</font>{文件名分隔符}使用：点

<font color=blue>［参考］</font>{文件名分隔符} 可选范围：中划线，下划线，点

<font color=green>［正例］</font>/data/logs/tsb/user/20170913.INFO.log

<font color=green>［正例］</font>/data/logs/tsb/user/2017091314.ERROR.log

## 日志输出

<font color=red>［强制］</font>不同级别日志通过配置分开输出。

<font color=green>［推荐］</font>对于不能通过配置作出分级别输出的工具组件，应将ERROR以上级别单独输出。

# 内容格式

## 日志内容

<font color=red>［强制］</font>简明扼要，无冗余

<font color=red>［强制］</font>关键业务必须可通过日志回溯请求，并定义明确的日志级别

<font color=red>［强制］</font>异常与错误必须记录日志，并定义明确的日志级别

<font color=red>［强制］</font>不允许将已捕获的异常栈随意丢进日志，应给出明确的级别和语义描述

<font color=red>［强制］</font>每一条日志内容必须包括且不限于以下内容：时间、进程ID、日志级别、日志内容。

## 日志格式

<font color=red>［强制］</font>每一条日志记录为一行

<font color=green>［推荐］</font>对于日志内容中有换行操作的，应计划处理为一行，否则日志收集之后将出现不可查看或分析问题

<font color=red>［强制］</font>单条日志内容格式为

   {时间点} {日志分隔符} {级别} {日志分隔符} {进程ID} {日志分隔符} [{线程名}] {日志分隔符} {日志内容}

<font color=red>［强制］</font>{时间点} 格式为：yyyy-MM-dd hh:mm:ss[.SSS] (年-月-日 小时:分钟:秒[.毫秒])

<font color=green>［推荐］</font>{日志分隔符} 使用：竖线

<font color=blue>［参考］</font>{日志分隔符} 可选范围：竖线，空格

<font color=green>［正例］</font>2017-09-13 19:35:54 | ERROR | 26922 | api error /api/getuserinfo 404

<font color=green>［正例］</font>2017-09-13 19:35:54 ERROR 26922 api error /api/getuserinfo 404

