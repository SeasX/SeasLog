<?php
/**
 * 邮件模板绑定数据
 *
 * @author ciogao@gmail.com
 * Date: 14-4-1 下午11:38
 */
namespace constant;

class mailTpl
{
    static private $tplKey = array('{?title}', '{?content}', '{?foot}');

    static private $tpl = '
        <html>
        <head>
        <title>SeasLog Analyzer</title>
        <style type="text/css">
        body {background-color:	#fff;margin:40px;font-family:Lucida Grande, Verdana, Sans-serif;font-size:12px;color:#000;}
        #content {border:1px solid #dddddd;background-color:#f5f5f5;padding:20px 20px 12px 20px;border-radius: 4px;}
        #foot {padding: 5px 20px 5px 20px;margin-top: 20px;}
        h1 {font-weight:normal;font-size:16px;color:#0088cc;margin:0 0 4px 0;}
        p {line-height: 20px;}
        .alert {padding: 8px 35px 8px 14px;margin-bottom: 20px;color: #c09853;text-shadow: 0 1px 0 rgba(255, 255, 255, 0.5);background-color: #fcf8e3;border: 1px solid #fbeed5;-webkit-border-radius: 4px;-moz-border-radius: 4px;border-radius: 4px;}
        .alert-info {color: #3a87ad;background-color: #d9edf7;border-color: #bce8f1;}
        .alert-error{color: #b94a48;background-color: #f2dede;border-color: #eed3d7;}
        </style>
        </head>
        <body>
            <h1>{?title}</h1>
            <div id="content">
                <p>{?content}</p>
            </div>
            <div id="foot" class="alert">
                <p>{?foot}</p>
                ---------------------------------------
                <br />SeasLog Analyzer
            </div>
        </body>
        </html>
    ';

    static public function processMail($title, $content, $foot)
    {
        return str_replace(self::$tplKey, array($title, $content, $foot), self::$tpl);
    }


}