#!/usr/bin/env python

import webbrowser
import time
try:
  import SimpleHTTPServer
  import thread
except:
  import http.server as SimpleHTTPServer
  import _thread as thread

def launch():
  time.sleep(1)
  webbrowser.open("http://localhost:8000/Presenter.html")

thread.start_new_thread(launch, ())

# start mini HTTP daemon.
SimpleHTTPServer.test(HandlerClass=SimpleHTTPServer.SimpleHTTPRequestHandler)
