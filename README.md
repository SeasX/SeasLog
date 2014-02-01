SeasLog
=======

Yat a log extension for PHP.

@author ciogao@gmail.com


### SeasLog目前提供了什么
* 在PHP项目中便捷、规范地记录log
* 可配置的默认log目录与模块
* 指定log目录与获取当前配置


### SeasLog的目标
* 便捷、规范的log记录
* 高效的海量log分析
* 可配置、多途径的log预警


### SeasLog编译安装
```conf
phpize

./configure --with-php-config=php-config

make

make install
```

### seaslog.ini
```conf
; configuration for php SeasLog module
extension=seaslog.so
seaslog.default_basepath = /log/seaslog-test
seaslog.default_logger = default
```

