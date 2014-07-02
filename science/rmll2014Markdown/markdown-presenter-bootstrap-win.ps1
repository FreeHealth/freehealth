function Download-File {
    param (
        [string]$url,
        [string]$file
    )
    Write-Host "Downloading $url to $file"
    $downloader = new-object System.Net.WebClient
    $downloader.DownloadFile($url, $file)
}

function Expand-ZipFile {
    param (
        [string]$file,
        [string]$destination
    )
    $shell = New-Object -ComObject shell.application
    $zip = $shell.NameSpace($file)
    foreach($item in $zip.items())
    {
        $shell.Namespace($destination).copyhere($item)
    }
}

$tagname = "1.1"
$zipname = "markdown-presenter-$tagname.zip"
$zippath = Join-Path (pwd).Path $zipname
Download-File -url "https://github.com/jsakamoto/MarkdownPresenter/releases/download/v.$tagname/$zipname" -file $zippath
Expand-ZipFile -file $zippath -destination (pwd).Path
sleep -Seconds 1
del $zippath

.\start-presenter-for-windows.bat
