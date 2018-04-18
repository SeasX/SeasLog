# PHP Re result

```php
/path/to/php --re seaslog

Extension [ <persistent> extension #44 SeasLog version 1.8.5 ] {

  - Dependencies {
  }

  - INI {
    Entry [ seaslog.default_basepath <SYSTEM> ]
      Current = '/var/log/www'
    }
    Entry [ seaslog.default_logger <SYSTEM> ]
      Current = 'default'
    }
    Entry [ seaslog.default_datetime_format <SYSTEM> ]
      Current = 'Y-m-d H:i:s'
    }
    Entry [ seaslog.default_template <SYSTEM> ]
      Current = '%T | %L | %P | %Q | %t | %M'
    }
    Entry [ seaslog.disting_folder <SYSTEM> ]
      Current = '1'
    }
    Entry [ seaslog.disting_type <SYSTEM> ]
      Current = '0'
    }
    Entry [ seaslog.disting_by_hour <SYSTEM> ]
      Current = '0'
    }
    Entry [ seaslog.use_buffer <SYSTEM> ]
      Current = '0'
    }
    Entry [ seaslog.buffer_size <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.buffer_disabled_in_cli <SYSTEM> ]
      Current = '0'
    }
    Entry [ seaslog.trace_notice <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.trace_warning <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.trace_error <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.trace_exception <SYSTEM> ]
      Current = '0'
    }
    Entry [ seaslog.level <ALL> ]
      Current = '8'
    }
    Entry [ seaslog.recall_depth <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.appender <SYSTEM> ]
      Current = '1'
    }
    Entry [ seaslog.appender_retry <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.remote_host <ALL> ]
      Current = '127.0.0.1'
    }
    Entry [ seaslog.remote_port <ALL> ]
      Current = '514'
    }
    Entry [ seaslog.remote_timeout <SYSTEM> ]
      Current = '1'
    }
    Entry [ seaslog.trim_wrap <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.throw_exception <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.ignore_warning <ALL> ]
      Current = '1'
    }
  }

  - Constants [16] {
    Constant [ string SEASLOG_VERSION ] { 1.8.5 }
    Constant [ string SEASLOG_AUTHOR ] { Chitao.Gao  [ neeke@php.net ] }
    Constant [ string SEASLOG_ALL ] { ALL }
    Constant [ string SEASLOG_DEBUG ] { DEBUG }
    Constant [ string SEASLOG_INFO ] { INFO }
    Constant [ string SEASLOG_NOTICE ] { NOTICE }
    Constant [ string SEASLOG_WARNING ] { WARNING }
    Constant [ string SEASLOG_ERROR ] { ERROR }
    Constant [ string SEASLOG_CRITICAL ] { CRITICAL }
    Constant [ string SEASLOG_ALERT ] { ALERT }
    Constant [ string SEASLOG_EMERGENCY ] { EMERGENCY }
    Constant [ integer SEASLOG_DETAIL_ORDER_ASC ] { 1 }
    Constant [ integer SEASLOG_DETAIL_ORDER_DESC ] { 2 }
    Constant [ integer SEASLOG_APPENDER_FILE ] { 1 }
    Constant [ integer SEASLOG_APPENDER_TCP ] { 2 }
    Constant [ integer SEASLOG_APPENDER_UDP ] { 3 }
  }

  - Functions {
    Function [ <internal:SeasLog> function seaslog_get_version ] {
    }
    Function [ <internal:SeasLog> function seaslog_get_author ] {
    }
  }

  - Classes [1] {
    Class [ <internal:SeasLog> class SeasLog ] {

      - Constants [0] {
      }

      - Static properties [0] {
      }

      - Static methods [22] {
        Method [ <internal:SeasLog> static public method setBasePath ] {

          - Parameters [1] {
            Parameter #0 [ <required> $base_path ]
          }
        }

        Method [ <internal:SeasLog> static public method getBasePath ] {
        }

        Method [ <internal:SeasLog> static public method setLogger ] {

          - Parameters [1] {
            Parameter #0 [ <required> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method getLastLogger ] {
        }

        Method [ <internal:SeasLog> static public method setRequestID ] {

          - Parameters [1] {
            Parameter #0 [ <required> $request_id ]
          }
        }

        Method [ <internal:SeasLog> static public method getRequestID ] {
        }

        Method [ <internal:SeasLog> static public method setDatetimeFormat ] {

          - Parameters [1] {
            Parameter #0 [ <required> $format ]
          }
        }

        Method [ <internal:SeasLog> static public method getDatetimeFormat ] {
        }

        Method [ <internal:SeasLog> static public method analyzerCount ] {

          - Parameters [3] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $log_path ]
            Parameter #2 [ <optional> $key_word ]
          }
        }

        Method [ <internal:SeasLog> static public method analyzerDetail ] {

          - Parameters [6] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $log_path ]
            Parameter #2 [ <optional> $key_word ]
            Parameter #3 [ <optional> $start ]
            Parameter #4 [ <optional> $limit ]
            Parameter #5 [ <optional> $order ]
          }
        }

        Method [ <internal:SeasLog> static public method getBuffer ] {
        }

        Method [ <internal:SeasLog> static public method getBufferEnabled ] {
        }

        Method [ <internal:SeasLog> static public method flushBuffer ] {
        }

        Method [ <internal:SeasLog> static public method log ] {

          - Parameters [4] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $message ]
            Parameter #2 [ <optional> $content ]
            Parameter #3 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method debug ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method info ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method notice ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method warning ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method error ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method critical ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method alert ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method emergency ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }
      }

      - Properties [0] {
      }

      - Methods [2] {
        Method [ <internal:SeasLog, ctor> public method __construct ] {
        }

        Method [ <internal:SeasLog, dtor> public method __destruct ] {
        }
      }
    }
  }
}
```

