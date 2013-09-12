<?PHP

$quotes = array();
if (!isset($_GET['stock1']) || !isset($_GET['stock2']) || !isset($_GET['stock3'])) die();
$stock = $_GET['stock1'];
$quote = curl_get('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock);
$quotes = array_merge($quotes, process_quotes($quote));

$stock = $_GET['stock2'];
$quote = curl_get('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock);
$quotes = array_merge($quotes, process_quotes($quote));

$stock = $_GET['stock3'];
$quote = curl_get('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock);
$quotes = array_merge($quotes, process_quotes($quote));

print json_encode($quotes);

// Obtain Quote Info
// $quote = file_get_contents('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock);
// or with curl
function curl_get($url){
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_REFERER, $url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/1.0");
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    $output = curl_exec($ch);
    curl_close($ch);
    return $output;
}

function process_quotes($quote) {
    $json_output = json_decode(utf8_decode($quote));
    if (!$json_output) die(); 

    $symb = $json_output->Data->Symbol;
    $last = $json_output->Data->LastPrice;
    $perc = $json_output->Data->ChangePercent;
//    $date = $json_output->Data->Timestamp;
	
    $result = array();
    $result[1] = $symb;
    $result[2] = array('I', round($last, 2) * 100);
    $result[3] = array('I', round($perc, 2) * 100);
    return $result;
}

/*
stdClass Object
(
    [Data] => stdClass Object
        (
            [Status] => SUCCESS
            [Name] => Facebook Inc
            [Symbol] => FB
            [LastPrice] => 31.91
            [Change] => -1.12
            [ChangePercent] => -3.3908567968514
            [Timestamp] => Fri May 25 16:00:05 UTC-04:00 2012
            [MarketCap] => 20214729720
            [Volume] => 37189630
            [ChangeYTD] => 0
            [ChangePercentYTD] => 0
            [High] => 32.95
            [Low] => 31.11
            [Open] => 32.9
        )

)*/
?>
