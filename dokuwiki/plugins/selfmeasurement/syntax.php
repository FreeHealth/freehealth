<?php
/**
 * DokuWiki Plugin selfmeasurement (Syntax Component)
 *
 * @license BSD-3 Clause http://www.gnu.org/licenses/bsd.html
 * @author  Eric Maeker, MD (fr) <eric.maeker@gmail.com>
 */

// must be run within Dokuwiki
if (!defined('DOKU_INC')) die();

if (!defined('DOKU_LF')) define('DOKU_LF', "\n");
if (!defined('DOKU_TAB')) define('DOKU_TAB', "\t");
if (!defined('DOKU_INC')) define('DOKU_INC',realpath(dirname(__FILE__).'/../../').'/');
if (!defined('DOKU_PLUGIN')) define('DOKU_PLUGIN',DOKU_INC.'lib/plugins/');

require_once DOKU_PLUGIN.'syntax.php';

class syntax_plugin_selfmeasurement extends DokuWiki_Syntax_Plugin {

    public static $lastFormId = 1;
    private $formId = 0;

    /** Version 1.00 - 26/02/2013 */
    function getInfo(){
        return array(
            'author' => 'Eric Maeker',
            'email'  => 'eric.maeker@gmail.com',
            'date'   => '2013-02-26',
            'name'   => 'selfmeasurement',
            'desc'   => 'Self Measurement of blood pressure assessment',
            'url'    => 'http://www.dokuwiki.org/wiki:plugins',
        );
    }

    public function getType() {
        // return 'FIXME: container|baseonly|formatting|substition|protected|disabled|paragraphs';
        return 'substition';
    }

    public function getPType() {
        // return 'FIXME: normal|block|stack';
        return 'normal';
    }

    public function getSort() {
        return 32;
    }

    /** 
     * Connect DokuWiki syntax:
     *     {{selfmeasurement}} 
     */
    public function connectTo($mode) {
        // $this->Lexer->addSpecialPattern('\[NOW\]',$mode,'plugin_now');
        $this->Lexer->addSpecialPattern('\{\{selfmeasurement\}\}',$mode,'plugin_selfmeasurement');
        // $this->Lexer->addSpecialPattern('\{\{selfmeasurement>[^}]*\}\}',$mode,'plugin_selfmeasurement');
        // $this->Lexer->addEntryPattern('<FIXME>',$mode,'plugin_selfmeasurement');
    }

    public function handle($match, $state, $pos, &$handler) {
        $data = array();
        $match = substr($match,18,-2); //strip markup from start and end
        // handle params
        $params = explode('|',$match);
        foreach($params as $param){
            $splitparam = explode('=', $param);
            $data[$splitparam[0]] = $splitparam[1];
        }
        return $data;
    }

    /**
     * Rendering function. If the rendering contains form values, send the report, otherwise add the form.
     */
    public function render($mode, &$renderer, $data) {
        //if ($mode != 'xhtml') return false;

        if (isset($_POST['submit-form-'.$this->formId])) {
            $this->_send_measurements($renderer);
            return true;
        }

        // include the form
        $form = file_get_contents(dirname(__FILE__)."/form.html");
        $form = str_replace("__FORM__", "action=\"".$_SERVER['REQUEST_URI']."#form-".$this->formId."\"f method=\"POST\"", $form);
        $form = str_replace("__SUBMIT_NAME__", "submit-form-".$this->formId, $form);
        $renderer->doc .= $form;
        return true;
    }

    /**
     * When the rendering is required with a validated form:
     * Create the report and send it by mail (according to the conf)
     */
    protected function _send_measurements(&$renderer) {
        $report_html = file_get_contents(dirname(__FILE__)."/report.html");
        $report_txt  = file_get_contents(dirname(__FILE__)."/report.txt");

        // replace all measurements from the post to the report
        for($i = 1; $i <= 18; $i++) {
            $find = "_S" . sprintf("%02s", $i) . "_";
            $post = "s" . $i;
            $report_html = str_replace($find, sprintf("%5.5s", $_POST[$post]), $report_html);
            $report_txt = str_replace($find, sprintf("%5.5s", $_POST[$post]), $report_txt);

            $find = "_D" . sprintf("%02s", $i) . "_";
            $post = "d" . $i;
            $report_html = str_replace($find, sprintf("%5.5s", $_POST[$post]), $report_html);
            $report_txt = str_replace($find, sprintf("%5.5s", $_POST[$post]), $report_txt);
        }

        // Compute day averages & full measurement averages
        $fullSystAverage = 0;
        $fullDiastAverage = 0;
        $fullSystNb = 0;
        $fullDiastNb = 0;

        for($i = 0; $i < 3; $i++) {
            $systAverage = 0;
            $diastAverage = 0;
            $systNb = 0;
            $diastNb = 0;

            for($j = 1; $j <= 6; $j++) {
                $nb = ($i*6 + $j);
                $post = "s" . $nb;
                $systAverage += $_POST[$post];
                if ($_POST[$post] != '')
                    $systNb++;

                $post = "d" . $nb;
                $diastAverage += $_POST[$post];
                if ($_POST[$post] != '')
                    $diastNb++;
            }
            if ($systNb == 0) {
                $systAverage = 0;
            } else {
                $fullSystAverage += $systAverage;
                $fullSystNb += $systNb;
                $systAverage = $systAverage / $systNb;
            }

            if ($diastNb == 0) {
                $diasAverage = 0;
            } else {
                $fullDiastAverage += $diastAverage;
                $fullDiastNb += $diastNb;
                $diastAverage = $diastAverage / $diastNb;
            }

            $day = ($i+1);
            $find = "_SYS_AV_DAY" . $day . "_";
            $report_html = str_replace($find, sprintf("%5.5s", $systAverage), $report_html);
            $report_txt = str_replace($find, sprintf("%5.5s", $systAverage), $report_txt);
            $find = "_DIA_AV_DAY" . $day . "_";
            $report_html = str_replace($find, sprintf("%5.5s", $diastAverage), $report_html);
            $report_txt = str_replace($find, sprintf("%5.5s", $diastAverage), $report_txt);
        }

        // Manage full measurements averages
        if ($fullSystNb == 0) {
            $fullSystAverage = 0;
        } else {
            $fullSystAverage = $fullSystAverage / $fullSystNb;
        }

        if ($fullDiastNb == 0) {
            $fullDiastAverage = 0;
        } else {
            $fullDiastAverage = $fullDiastAverage / $fullDiastNb;
        }
        $find = "_SYS_AV_";
        $report_html = str_replace($find, sprintf("%8.8s", $fullSystAverage), $report_html);
        $report_txt = str_replace($find, sprintf("%8.8s", $fullSystAverage), $report_txt);
        $find = "_DIA_AV_";
        $report_html = str_replace($find, sprintf("%8.8s", $fullDiastAverage), $report_html);
        $report_txt = str_replace($find, sprintf("%8.8s", $fullDiastAverage), $report_txt);
        $find = "_SYS_NB_";
        $report_html = str_replace($find, sprintf("%8.8s", $fullSystNb), $report_html);
        $report_txt = str_replace($find, sprintf("%8.8s", $fullSystNb), $report_txt);
        $find = "_DIA_NB_";
        $report_html = str_replace($find, sprintf("%8.8s", $fullDiastNb), $report_html);
        $report_txt = str_replace($find, sprintf("%8.8s", $fullDiastNb), $report_txt);

        // Replace all tokens
        // __NAMES__ __DATE__
        $find = "__NAMES__";
        $report_html = str_replace($find, $_POST['names'], $report_html);
        $report_txt = str_replace($find, $_POST['names'], $report_txt);
        $find = "__DATE__";
        $date = $_POST['date'];
        if ($date == "")
            $date = date("Y-m-d", $_SERVER['REQUEST_TIME']);
        $report_html = str_replace($find, $date, $report_html);
        $report_txt = str_replace($find, $date, $report_txt);

        // Prepare mail (To, Names, Subject)
        $lf = "\r\n";  // LineFeed
        $to  = $this->getConf('automeasurement_sendto');
        $name = str_replace(" ", "_", $_POST['names']);
        if ($name == "")
            $name = "Inconnu";
        $subject = '[Auto-mesures] - ' . $name;
        $filename = 'selfmeasurement_' . $name . "_" . $date;

        // Prepare mail (Headers, Attachement)
        $random_hash = "DokuWiki-SelfMeasurement--" . md5(date('r', time()));

        $headers  = "From: " . $to . $lf;
        $headers .= "Content-Type: multipart/mixed; boundary=\"".$random_hash."\"" . $lf;
        $headers .= "Reply-To: " . $to . $lf;
        $headers .= "Date: " . date('r', $_SERVER['REQUEST_TIME']) . $lf;
        $headers .= "Mime-Version: 1.0" . $lf;
        $headers .= $lf;

        // TODO: add style.css in the report_html var (manage the __text__ tokens)
        $attachment_html = chunk_split(base64_encode($report_html));
        //$attachment_txt = chunk_split(base64_encode($report_txt));

        // Prepare mail (Body)
        $output  = "--$random_hash" . $lf;
        $output .= "Content-Transfer-Encoding: 7bit" . $lf;
        $output .= "Content-Type: text/plain; charset='utf-8'" . $lf;
        $output .= $lf;
        $output .= $report_txt . $lf;
        $output .= $lf;
        $output .= "--$random_hash" . $lf;
        $output .= "Content-Disposition: attachment" . $lf;
        $output .= "Content-Type: text/html; name=$filename.html" . $lf;
        $output .= "Content-Transfer-Encoding: base64" . $lf;
        $output .= $lf;
        $output .= $attachment_html;
        $output .= $lf;
        $output .= "--$random_hash" . $lf;
        $output .= "Content-Disposition: attachment" . $lf;
        $output .= "Content-Type: text/plain; charset='utf-8'; name=$filename.txt" . $lf;
        $output .= "Content-Transfer-Encoding: quoted-printable" . $lf;
        $output .= $lf;
        $output .= $report_txt; //$attachment_txt;
        $output .= "--$random_hash--" . $lf;

        if (mail($to, $subject, $output, $headers)) {
            $renderer->doc .= "<h1>Les r&eacute;sultats ont &eacute;t&eacute; transmis. Inutile d'imprimer la page</h1>"; // TODO: use a conf/lang instead of magic number
        } else {
            $renderer->doc .= "<h1>Erreur: Les r&eacute;sultats n'ont pas &eacute;t&eacute; transmis. Veuillez imprimer la page.</h1>"; // TODO: use a conf/lang instead of magic number
        }

        $renderer->doc .= $report_html;

        return true;
    }
}

// vim:ts=4:sw=4:et:
