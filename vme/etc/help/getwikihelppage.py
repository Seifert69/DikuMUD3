import pdb;
import sys
import json
import urllib2

# MS2020
# use this script to retrieve a help file from the Wiki.
# Will get the page in wiki.dikumud.net/Help/thepage
# and save it to a .hlp file.
#
# The idea is that we can maintain help files in the Wiki
# rather than text files on each MUD
#
# Example: 
#   python getwikihelppage.py Help
#
# will retrieve wiki.dikumud.net/Help/Help and save it as help.hlp
#

if len(sys.argv) != 2:
    print('Please provide name of help item to retrieve')
    sys.exit()
    quit()

print("Getting Wiki page from wiki.dikumud.net: " + sys.argv[1])

try: 
    j = urllib2.urlopen('https://wiki.dikumud.net/api.php?action=parse&format=json&page=Help/'+sys.argv[1])
except urllib2.HTTPError, e:
    print('HTTPError = ' + str(e.code))
    sys.exit()
except urllib2.URLError, e:
    print('URLError = ' + str(e.reason))
    sys.exit()
except httplib.HTTPException, e:
    print('HTTPException')
    sys.exit()
except Exception:
    import traceback
    print('generic exception: ' + traceback.format_exc())
    sys.exit()

data = json.load(j)

if "error" in data:
    print("Error response:")
    if "info" in data["error"]:
        print(data["error"]["info"])
    else:
        print(data["error"])
    sys.exit()

#print json.dumps(data, indent=4)
#print data["parse"]["text"]

mystr = data["parse"]["text"]["*"]

# Raw string extracted:
#print("========================")
#print("========================")

print("Transforming Wiki data response")

#
# Remove HTML comments <!-- -->
#
objs = mystr.split('<!--')

if len(objs) > 1:
    mystr = objs[0]
    for i in range(1, len(objs)):
        mystr += objs[i].split('-->')[1]

#
# Remove header junk
#
objs = mystr.split('<h1>')

if len(objs) > 1:
    mystr = objs[0]
    o2 = objs[1].split('</h1>')

    #
    # Remove all < >
    #
    mystr += "<h1>"
    o3 = o2[0].split('<')

    if len(o3) > 1:
        mystr += o3[0]
        for i in range(1, len(o3)):
            mystr += o3[i].split('>')[1]

    mystr += "</h1>"+o2[1]


#
# Remove [edit] from <h1>
#
objs = mystr.split('<h1>')

if len(objs) > 1:
    mystr = objs[0]
    o2 = objs[1].split('</h1>')

    #
    # Remove all < >
    #
    mystr += "<h1>"
    o3 = o2[0].split('[edit]')
    mystr += o3[0]

    mystr += "</h1>"+o2[1]


#
# Morph <a href="/index.php?title=Help/Idea&amp;action=edit&amp;redlink=1" class="new" title="Help/Idea (page does not exist)">idea</a>
# to {idea}
#
objs = mystr.split('<a href="/index.php?title=Help/')

if len(objs) > 1:
    #pdb.set_trace()
    mystr = objs[0]
    for i in range(1, len(objs)):
        o2 = objs[i].split('</a>')
        mystr += "{" + o2[0].split('>')[1] + "}" + o2[1]


#
# Morph {text} to <a cmd='#'>text</a>
#
objs = mystr.split('{')

if len(objs) > 1:
    #pdb.set_trace()
    mystr = objs[0]
    for i in range(1, len(objs)):
        o2 = objs[i].split('}')
        if len(o2) > 1:
            mystr += "<a cmd='#'>" + o2[0] + "</a>" + o2[1]
        else:
            print "error: missing end curly bracket on string {"+objs[i]
            mystr += objs[i]

myfile = sys.argv[1].lower() + ".hlp"
print("Saving result to " + myfile)

text_file = open(myfile, "w")
text_file.write(mystr)
text_file.close()

print(str(len(mystr)) +" bytes written.")
