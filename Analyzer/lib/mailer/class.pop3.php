<?php
/**
 * Class POP3
 */
namespace lib\mailer;

class POP3
{
    /**
     * Default POP3 port
     * @var int
     */
    public $POP3_PORT = 110;

    /**
     * Default Timeout
     * @var int
     */
    public $POP3_TIMEOUT = 30;

    /**
     * POP3 Carriage Return + Line Feed
     * @var string
     */
    public $CRLF = "\r\n";

    /**
     * Displaying Debug warnings? (0 = now, 1+ = yes)
     * @var int
     */
    public $do_debug = 2;

    /**
     * POP3 Mail Server
     * @var string
     */
    public $host;

    /**
     * POP3 Port
     * @var int
     */
    public $port;

    /**
     * POP3 Timeout Value
     * @var int
     */
    public $tval;

    /**
     * POP3 Username
     * @var string
     */
    public $username;

    /**
     * POP3 Password
     * @var string
     */
    public $password;

    /////////////////////////////////////////////////
    // PROPERTIES, PRIVATE AND PROTECTED
    /////////////////////////////////////////////////

    private $pop_conn;
    private $connected;
    private $error; //  Error log array

    /**
     * Constructor, sets the initial values
     * @access public
     * @return POP3
     */
    public function __construct()
    {
        $this->pop_conn  = 0;
        $this->connected = FALSE;
        $this->error     = NULL;
    }

    /**
     * Combination of public events - connect, login, disconnect
     * @access public
     * @param string $host
     * @param bool|int $port
     * @param bool|int $tval
     * @param string $username
     * @param string $password
     * @param int $debug_level
     * @return bool
     */
    public function Authorise($host, $port = FALSE, $tval = FALSE, $username, $password, $debug_level = 0)
    {
        $this->host = $host;

        //  If no port value is passed, retrieve it
        if ($port == FALSE) {
            $this->port = $this->POP3_PORT;
        } else {
            $this->port = $port;
        }

        //  If no port value is passed, retrieve it
        if ($tval == FALSE) {
            $this->tval = $this->POP3_TIMEOUT;
        } else {
            $this->tval = $tval;
        }

        $this->do_debug = $debug_level;
        $this->username = $username;
        $this->password = $password;

        //  Refresh the error log
        $this->error = NULL;

        //  Connect
        $result = $this->Connect($this->host, $this->port, $this->tval);

        if ($result) {
            $login_result = $this->Login($this->username, $this->password);

            if ($login_result) {
                $this->Disconnect();

                return TRUE;
            }

        }

        //  We need to disconnect regardless if the login succeeded
        $this->Disconnect();

        return FALSE;
    }

    /**
     * Connect to the POP3 server
     * @access public
     * @param string $host
     * @param bool|int $port
     * @param integer $tval
     * @return boolean
     */
    public function Connect($host, $port = FALSE, $tval = 30)
    {
        //  Are we already connected?
        if ($this->connected) {
            return TRUE;
        }

        /*
        On Windows this will raise a PHP Warning error if the hostname doesn't exist.
        Rather than supress it with @fsockopen, let's capture it cleanly instead
        */

        set_error_handler(array(&$this, 'catchWarning'));

        //  Connect to the POP3 server
        $this->pop_conn = fsockopen($host, //  POP3 Host
            $port, //  Port #
            $errno, //  Error Number
            $errstr, //  Error Message
            $tval); //  Timeout (seconds)

        //  Restore the error handler
        restore_error_handler();

        //  Does the Error Log now contain anything?
        if ($this->error && $this->do_debug >= 1) {
            $this->displayErrors();
        }

        //  Did we connect?
        if ($this->pop_conn == FALSE) {
            //  It would appear not...
            $this->error = array(
                'error'  => "Failed to connect to server $host on port $port",
                'errno'  => $errno,
                'errstr' => $errstr
            );

            if ($this->do_debug >= 1) {
                $this->displayErrors();
            }

            return FALSE;
        }

        //  Increase the stream time-out

        //  Check for PHP 4.3.0 or later
        if (version_compare(phpversion(), '5.0.0', 'ge')) {
            stream_set_timeout($this->pop_conn, $tval, 0);
        } else {
            //  Does not work on Windows
            if (substr(PHP_OS, 0, 3) !== 'WIN') {
                socket_set_timeout($this->pop_conn, $tval, 0);
            }
        }

        //  Get the POP3 server response
        $pop3_response = $this->getResponse();

        //  Check for the +OK
        if ($this->checkResponse($pop3_response)) {
            //  The connection is established and the POP3 server is talking
            $this->connected = TRUE;
            return TRUE;
        }

    }

    /**
     * Login to the POP3 server (does not support APOP yet)
     * @access public
     * @param string $username
     * @param string $password
     * @return boolean
     */
    public function Login($username = '', $password = '')
    {
        if ($this->connected == FALSE) {
            $this->error = 'Not connected to POP3 server';

            if ($this->do_debug >= 1) {
                $this->displayErrors();
            }
        }

        if (empty($username)) {
            $username = $this->username;
        }

        if (empty($password)) {
            $password = $this->password;
        }

        $pop_username = "USER $username" . $this->CRLF;
        $pop_password = "PASS $password" . $this->CRLF;

        //  Send the Username
        $this->sendString($pop_username);
        $pop3_response = $this->getResponse();

        if ($this->checkResponse($pop3_response)) {
            //  Send the Password
            $this->sendString($pop_password);
            $pop3_response = $this->getResponse();

            if ($this->checkResponse($pop3_response)) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            return FALSE;
        }
    }

    /**
     * Disconnect from the POP3 server
     * @access public
     */
    public function Disconnect()
    {
        $this->sendString('QUIT');

        fclose($this->pop_conn);
    }

    /////////////////////////////////////////////////
    //  Private Methods
    /////////////////////////////////////////////////

    /**
     * Get the socket response back.
     * $size is the maximum number of bytes to retrieve
     * @access private
     * @param integer $size
     * @return string
     */
    private function getResponse($size = 128)
    {
        $pop3_response = fgets($this->pop_conn, $size);

        return $pop3_response;
    }

    /**
     * Send a string down the open socket connection to the POP3 server
     * @access private
     * @param string $string
     * @return integer
     */
    private function sendString($string)
    {
        $bytes_sent = fwrite($this->pop_conn, $string, strlen($string));

        return $bytes_sent;
    }

    /**
     * Checks the POP3 server response for +OK or -ERR
     * @access private
     * @param string $string
     * @return boolean
     */
    private function checkResponse($string)
    {
        if (substr($string, 0, 3) !== '+OK') {
            $this->error = array(
                'error'  => "Server reported an error: $string",
                'errno'  => 0,
                'errstr' => ''
            );

            if ($this->do_debug >= 1) {
                $this->displayErrors();
            }

            return FALSE;
        } else {
            return TRUE;
        }

    }

    /**
     * If debug is enabled, display the error message array
     * @access private
     */
    private function displayErrors()
    {
        echo '<pre>';

        foreach ($this->error as $single_error) {
            print_r($single_error);
        }

        echo '</pre>';
    }

    /**
     * Takes over from PHP for the socket warning handler
     * @access private
     * @param integer $errno
     * @param string $errstr
     * @param string $errfile
     * @param integer $errline
     */
    private function catchWarning($errno, $errstr, $errfile, $errline)
    {
        $this->error[] = array(
            'error'  => "Connecting to the POP3 server raised a PHP warning: ",
            'errno'  => $errno,
            'errstr' => $errstr
        );
    }

    //  End of class
}