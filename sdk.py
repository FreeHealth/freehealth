#!/usr/bin/python
#----------------
# This file is part of Monkey Studio project http://www.monkeystudio.org/

from optparse import OptionParser
import os
from sys import exit
import sys
import glob
 
def pathsplit(path):
    return path.split(os.path.sep)
 
def commonpath(l1, l2, common=[]):
    if len(l1) < 1: return (common, l1, l2)
    if len(l2) < 1: return (common, l1, l2)
    if l1[0] != l2[0]: return (common, l1, l2)
    return commonpath(l1[1:], l2[1:], common+[l1[0]])
 
def relpath(p1, p2):
    (common,l1,l2) = commonpath(pathsplit(p1), pathsplit(p2))
    p = []
    if len(l1) > 0:
        p = [ '../' * len(l1) ]
    p = p + l2
    return os.path.join( *p )
 
def ask_confirmation(path):
    while True:
        print path+" exists, should I :"
        print "1 : remove and recreate"
        print "2 : update"
        print "3 : cancel"
 
        answer = sys.stdin.readline()
 
        try:
            answer = int(answer)
        except ValueError:
            print "Must be a number between 1 and 31\n"
            continue
 
        if answer >= 1 and answer <= 3:
            return answer
        print "Must be a number between 1 and 3\n"
 
def create_dir(dest):
    if not os.path.exists(dest):
	os.makedirs(dest)
	return
    return

    reps = pathsplit(dest)
    mkrep = ""
 
    for rep in reps:
	print rep + " rep!"
        mkrep = os.path.join(mkrep, rep)
        if os.path.exists(mkrep):
            ans = ask_confirmation(mkrep)
            if ans == 1:
                for root, dirs, files in os.walk(mkrep, topdown=False):
                    for name in files:
                        os.remove(os.path.join(root, name))
                    for name in dirs:
                        os.rmdir(os.path.join(root, name))
                os.rmdir(root)
 
            if ans == 2:
                continue
            if ans == 3:
                exit()
 
        os.mkdir(mkrep)
 
def get_h(source):
    return glob.glob(source+"/*.h")
 
def create_h(source, destination):
    files = get_h(source)
    inc_path = relpath(os.path.abspath(destination), os.path.abspath(source))
    create_dir(destination)
 
    for h_file in files:
        h_file = os.path.basename(h_file)
 
        if os.path.exists(os.path.join(destination, h_file)):
            ans = ask_confirmation(os.path.join(destination, h_file))
 
            if ans == 1:
                os.remove(os.path.join(destination, h_file))
                f = open(os.path.join(destination, h_file), "w")
                f.write("#include <" + os.path.join(inc_path,h_file) + ">\n")
            if ans == 2:
                f = open(os.path.join(destination, h_file), "r")
                text = f.read()
                f.close
                os.remove(os.path.join(destination, h_file))
                f = open(os.path.join(destination, h_file), "w")
                f.write("#include <" + os.path.join(inc_path,h_file) + ">\n")
                f.write(text)
            if ans == 3:
                exit()
        else:        
            f = open(os.path.join(destination, h_file), "w")
            f.write("#include <" + os.path.join(inc_path,h_file) + ">\n")
 
        f.close
 
if __name__ == "__main__":
    parser = OptionParser()
 
    parser.add_option("-d", "--destination-dir", dest="destination", help="destination directory")
    parser.add_option("-s", "--source-dir", dest="source", help="source directory")
 
    (options, args) = parser.parse_args()
 
    if (not options.destination) and (not options.source):
        print "Destination and source required."
        parser.print_help()
        exit()
 
    create_h(options.source, options.destination)
