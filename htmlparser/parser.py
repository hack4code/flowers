#!/bin/python 

from lxml import etree
from StringIO import StringIO
import re

def dfs(nodes, r, t):
    for node in nodes:
        cname = node.get("class")
        if -1 < cname.find("flor"):
            key = cname.split(' ')[1].strip()
            print "{ " + str(r + flowers[key][1]) + ", " + str(t + flowers[key][2]) + ", " + str(flowers[key][0]) + " },"
        elif -1 < cname.find("rama"):
            key = cname.strip()
            dfs(node.xpath("div"), r + branches[key][0], t + branches[key][1])


f = open("index.html", 'r')
html = f.read()

flowers = {}
branches = {}
parser = etree.HTMLParser()
tree = etree.parse(StringIO(html), parser)

hroot = tree.xpath("/html/head/style")
for l in hroot[0].text.split('\n'):
    l = l.strip()
    m = re.match(r"^\.r\d{1,2}f\d{1,2}", l)
    if m:
        low = l.index('{')
        high = l.index('}')
        if low > 0 and high > 0:
            key = m.group(0)[1:]
            flowers[key] = []
            for w in l[low+1:high].split(';'):
                if 0 < w.find("height"):
                    flowers[key].append(int(w.split(':')[1][:-2]))
                elif 0 < w.find("right"):
                    flowers[key].append(int(w.split(':')[1][:-2]))
                elif 0 < w.find("top"):
                    flowers[key].append(int(w.split(':')[1][:-2]))
                    break;
    m = re.match(r"^\.rama\d{1,3}", l)
    if m and 0 < l.find("right"):
        low = l.index('{')
        high = l.index('}')
        key = m.group(0)[1:]
        branches[key] = []
        for w in l[low+1:high].split(';'):
            if 0 < w.find("right"):
                branches[key].append(int(w.split(':')[1][:-2]))
            elif 0 < w.find("top"):
                branches[key].append(int(w.split(':')[1][:-2]))
                break;

        
            
mroot = tree.xpath("/html/body/div[1]/div[1]")
nodes= mroot[0].xpath("div")
dfs(nodes, -100, 0)

