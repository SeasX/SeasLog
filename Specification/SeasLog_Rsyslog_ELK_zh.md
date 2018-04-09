## SeasLog 日志收集

日志处理过程一般是 `输出日志 -> 收集日志 -> 分析日志 -> 存储日志 -> 后台管理`

本文将使用:
```
输出日志: SeasLog

收集日志: Rsyslog, Filebeat, Logstash

分析日志: Rsyslog, Logstash

存储日志: Elasticsearch
```
可自由搭配结合

### SeasLog 输出日志到 Rsyslog

本文统一 SeasLog 模板为 `seaslog.default_template = "%T | %L | %P | %Q | %t | %M"`

#### 1. 使用 File

1. 修改 SeasLog 配置为 File 输出

```conf
;日志存储介质 1File 2TCP 3UDP (默认为1)
seaslog.appender = 1

;默认log根目录
seaslog.default_basepath = "/var/log/www"

;默认logger目录
seaslog.default_logger = "default"
```

2. 修改 Rsyslog 配置开启 [imfile](http://www.rsyslog.com/doc/v8-stable/configuration/modules/imfile.html) 输入模块

```conf
#载入 imfile 模块
module(load="imfile" PollingInterval="10")

#使用 imfile 模块监听 `/var/log/www/default/` 文件夹下 .log 文件
input(type="imfile"
    File="/var/log/www/default/*.log"
    Tag="tag1"
    Severity="error"
    Facility="local7")
```

#### 2. 使用 TCP/UDP

SeasLog 中使用 [RFC5424](https://tools.ietf.org/html/rfc5424) 规范远程输出日志

输出日志格式为 `<{PRI}>1 {time_RFC3339} {host_name} {domain_port} {process_id} {logger} {log_message}`

具体使用 TCP 还是 UDP 根据业务需求来定, 下面使用 TCP 为例

1. 服务端: 修改 Rsyslog 配置开启 [imtcp](http://www.rsyslog.com/doc/v8-stable/configuration/modules/imtcp.html) 输入模块

```conf
#载入 imtcp 模块
module(load="imtcp")

#使用 imtcp 模块监听 514 端口作为输入
input(type="imtcp" port="514")
```

2. 客户端: 修改 SeasLog 配置使用 TCP

```conf
;日志存储介质 1File 2TCP 3UDP (默认为1)
seaslog.appender = 2

;接收ip 默认127.0.0.1 (当使用TCP或UDP时必填)
seaslog.remote_host = "192.168.0.2"

;接收端口 默认514 (当使用TCP或UDP时必填)
seaslog.remote_port = 514
```

### Rsyslog 处理日志

#### 1. 在未定义 template 的时候, Rsyslog 会使用默认模板对日志进行格式化

例如 rsyslogd 8.16.0

默认模板是: [`RSYSLOG_TraditionalFileFormat`](https://www.rsyslog.com/doc/v8-stable/configuration/templates.html)

定义: `$template TraditionalFileFormat,"%TIMESTAMP% %HOSTNAME% %syslogtag%%msg:::sp-if-no-1st-sp%%msg:::drop-last-lf%\n" `

%rawmsg% 是

`<14>1 2018-04-08T10:25:02+08:00 whj-desktop cli 6838 default 2018-04-08 10:25:02 | INFO | 6838 | 5ac97d7e0df1b | 1523154302.57 | i am cli test seaslog rsyslog`

%msg% 是

`Apr  8 10:23:40 whj-desktop cli[5551] 2018-04-08 10:23:40 | INFO | 5551 | 5ac97d2c6cf06 | 1523154220.446 | i am cli test seaslog rsyslog`

更多 Rsyslog [properties](http://www.rsyslog.com/doc/v8-stable/configuration/properties.html)

#### 2. Rsyslog 输出到文件

配置 [template](http://www.rsyslog.com/doc/v8-stable/configuration/templates.html)

```conf
#定义一个模板将日志打入 seaslog_%$year%%$month%%$day%.log
$template logfile,    "/var/log/seaslog_%$year%%$month%%$day%.log"

#定义一个模板对日志内容进行格式化
template(name="logformat" type="string" string="app-name: %APP-NAME%\nmsgid: %MSGID% \nmsg: %msg% \nrawmsg: %rawmsg% \n\n")

#使用contains 搜索含有 `seaslog` 关键字的日志并使用上面定义的那两个模板
:msg,contains,        "seaslog"            ?logfile;logformat
```

TCP/UDP 输出, Rsyslog 的 rawmsg 原始日志格式为[RFC5424](https://tools.ietf.org/html/rfc5424) 规范 `<{PRI}>1 {time_RFC3339} {host_name} {domain_port} {process_id} {logger} {log_message}`

```conf
app-name: cli
msgid: default 
msg: 2018-04-08 10:22:19 | INFO | 3748 | 5ac97cdba4926 | 1523154139.674 | i am cli test seaslog rsyslog 
rawmsg: <14>1 2018-04-08T10:22:19+08:00 whj-desktop cli 3748 default 2018-04-08 10:22:19 | INFO | 3748 | 5ac97cdba4926 | 1523154139.674 | i am cli test seaslog rsyslog 
```

File 输出, Rsyslog 采集到的日志格式是 `seaslog.default_template`

```conf
app-name: tag1
msgid: - 
msg: 2018-04-08 10:34:14 | INFO | 15597 | 5ac97fa6330a9 | 1523154854.209 | i am cli test seaslog rsyslog 
rawmsg: 2018-04-08 10:34:14 | INFO | 15597 | 5ac97fa6330a9 | 1523154854.209 | i am cli test seaslog rsyslog 
```


#### 3. Rsyslog 输出到 Logstash 和 elasticsearch

1. 配置 Logstash 的 input 为 tcp, 端口 5555

```conf
input {
    tcp {
        type => "seaslog"
        port => "5555"
    }
}
```

2. 设置 Rsyslog 的 filter 识别 RFC5424, [grokdebug](http://grokdebug.herokuapp.com/)

```
filter {
  if [type] == "seaslog" {
    grok {
      match => { "message" => "%{SYSLOG5424PRI}%{NONNEGINT:syslog5424_ver} +(?:%{TIMESTAMP_ISO8601:syslog5424_ts}|-) +(?:%{HOSTNAME:syslog5424_host}|-) +(?:%{NOTSPACE:syslog5424_app}|-) +(?:%{NOTSPACE:syslog5424_proc}|-) +(?:%{WORD:syslog5424_msgid}|-) +%{GREEDYDATA:syslog5424_msg}" }
    }
  }
}
```
3. 配置 Logstash 的 output 到 elasticsearch

```conf
output{
    if [type] == "seaslog" {
        elasticsearch {
            hosts => ["127.0.0.1:9200"] 
            index => "rsyslog-%{+YYYY.MM.dd}"
        }
    }
}
```
4. 将 Rsyslog 日志重定向到 Logstash

```conf
:msg,contains 'seaslog' @@192.168.0.2:5555
```

5. Rsyslog 会将接收到的 msg 经过 filter 过滤发送如下格式数据到 elasticsearch 储存

```conf
{
      "syslog5424_ver" => "1",
             "message" => "<14>1 2018-04-08T10:13:24+08:00 whj-desktop cli 27431 default 2018-04-08 10:13:24 | INFO | 27431 | 5ac97ac483522 | 1523153604.538 | i am cli test seaslog rsyslog",
                "type" => "seaslog",
      "syslog5424_app" => "cli",
      "syslog5424_msg" => "2018-04-08 10:13:24 | INFO | 27431 | 5ac97ac483522 | 1523153604.538 | i am cli test seaslog rsyslog",
     "syslog5424_proc" => "27431",
    "syslog5424_msgid" => "default",
          "@timestamp" => 2018-04-08T02:13:24.545Z,
                "port" => 33850,
       "syslog5424_ts" => "2018-04-08T10:13:24+08:00",
      "syslog5424_pri" => "14",
            "@version" => "1",
                "host" => "127.0.0.1",
     "syslog5424_host" => "whj-desktop"
}

```
### 使用 Filebeat 收集日志
收集 `/var/log/seaslog_*.log` 的日志并输出到 logstash

```yml
- type: log
  enabled: true
  paths:
    - /var/log/seaslog_*.log
output.logstash:
  hosts: ["localhost:5044"]
  ssl.certificate_authorities: ["/home/whj/logstash-beats.crt"]
```
配置 logstash 的 input 为 beats
```conf
input {
  beats {
    port => 5044
    ssl => true
    ssl_certificate => "/etc/pki/tls/certs/logstash-beats.crt"
    ssl_key => "/etc/pki/tls/private/logstash-beats.key"
  }
}
```
### 直接 SeasLog TCP 输出到 logstash

客户端: 修改 SeasLog 配置使用 TCP

```conf
;日志存储介质 1File 2TCP 3UDP (默认为1)
seaslog.appender = 2

;接收ip 默认127.0.0.1 (当使用TCP或UDP时必填)
seaslog.remote_host = "192.168.0.2"

;接收端口 默认514 (当使用TCP或UDP时必填)
seaslog.remote_port = 5555
```
