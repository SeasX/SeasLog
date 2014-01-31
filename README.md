SeasLog
=======

Yat a log extension for PHP.

phpize

./configure --with-php-config=php-config

make

make install


----seaslog.ini----
; configuration for php SeasLog module
extension=seaslog.so
seaslog.default_basepath = /log/seaslog-test
seaslog.default_logger = default
