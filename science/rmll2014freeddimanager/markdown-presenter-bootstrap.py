import webbrowser
try:
    import SimpleHTTPServer
    import thread
    import urllib
except:
    import http.server as SimpleHTTPServer
    import _thread as thread
    import urllib.request as urllib
import time
from zipfile import ZipFile
import os

tagname = "1.1"
zipname = "markdown-presenter-" + tagname + ".zip"
zippath = "./" + zipname
url = "https://github.com/jsakamoto/MarkdownPresenter/releases/download/v." + tagname + "/" + zipname

# download zip archive of Markdown Presenter.
urllib.urlretrieve(url, zippath)

# ectract the zip.
zfile = ZipFile(zippath)
zfile.extractall(".")
zfile.close()

# clean up zip.
os.remove(zippath)

# launch default web browser to open Markdown Presenter
# after one shot timer to wait for warming up HTTP daemon.
def launch():
    time.sleep(1)
    webbrowser.open("http://localhost:8000/Presenter.html")

thread.start_new_thread(launch, ())

# start mini HTTP daemon.
SimpleHTTPServer.test(HandlerClass=SimpleHTTPServer.SimpleHTTPRequestHandler)
