<?php
/**
 * 预警
 *
 * @author ciogao@gmail.com
 * Date: 14-3-11 上午10:51
 */
namespace lib;

use constant\config as config;

class alarmer
{
    private static $self = NULL;

    public static function instanse()
    {
        if (self::$self == NULL) self::$self = new self;

        return self::$self;
    }

    private static $_config;

    public function __construct()
    {
        self::$_config = config::getAnalyzEmail();
    }

    public function SendEmail($subject, $content, $to = array(), $cc = array(), $bcc = array(), $attachments = array())
    {
        $mail = new mailer\PHPMailer();
        $mail->IsSMTP();

        $mail->CharSet  = 'UTF-8';
        $mail->Encoding = 'base64';
        $mail->SMTPAuth = TRUE;
        $mail->Host     = self::$_config['smtp_host'];
        $mail->Port     = self::$_config['smtp_port'];
        $mail->Username = self::$_config['smtp_user'];
        $mail->Password = self::$_config['smtp_pwd'];
        $mail->SetFrom(self::$_config['mail_from'], "=?UTF-8?B?" . base64_encode(self::$_config['mail_from']) . "?=");
        $mail->Subject = "=?UTF-8?B?" . base64_encode(self::$_config['subject_pre'] . '-' . $subject) . "?=";
        $mail->MsgHTML($content);
        $mail->IsHTML(TRUE);

        if (count($attachments) > 0) {
            foreach ($attachments as $attachment) {
                $mail->AddAttachment($attachment);
            }
        }

        $to  = array_merge(self::$_config['mail_to'], $to);
        $cc  = array_merge(self::$_config['mail_cc'], $cc);
        $bcc = array_merge(self::$_config['mail_bcc'], $bcc);

        if (count($to) > 0) {
            foreach ($to as $address) {
                if (!empty($address)) $mail->AddAddress($address);
            }
        }

        if (count($cc) > 0) {
            foreach ($cc as $address) {
                if (!empty($address)) $mail->AddCC($address);
            }
        }

        if (count($bcc) > 0) {
            foreach ($bcc as $address) {
                if (!empty($address)) $mail->AddBCC($address);
            }
        }

        try {
            $sendResult = $mail->Send();
            if ($sendResult) {
                \SeasLog::info('报警邮件发送成功', array(), 'SeasLogAnalyzer');
            }

            return $sendResult;
        } catch (mailer\phpmailerException $e) {
            \Seaslog::error('报警邮件发送失败。{errorInfo}', array('{errorInfo}' => $e->getMessage()), 'SeasLogAnalyzer');

            return FALSE;
        }

        return TRUE;
    }
}