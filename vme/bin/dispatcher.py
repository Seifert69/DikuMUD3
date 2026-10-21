import threading
import os
import errno
import json
import re
import time

# vme listens to pipeMUD for messages from e.g. muddiscord.py. 
#     each integration should write to pipeMUD.
#
# dispathcer.py listens to pipeDispatcher for messages from the MUD server.
#     Only the MUD server should write to pipeDispatcher
#
# muddiscord.py listens to pipeDiscord for messages from the dispatcher
#     Only the dispatcher should write to pipeDiscord
#     muddiscord will write messages from discord to pipeMUD
#
# Maintains open writable named pipes to each listener we want to relay to. 
# E.g. we can relay messages from the MUD server to pipeDiscord or pipeGit.
# Another python program would e.g. listen to messages on pipeDiscord
#
# To test, run program 'python3 dispatcher.py' in the bin/ folder and try
# this shell command: printf "discord Papi says hi\n" > pipeDispatcher. 
# This would trigger this script as if the MUD server had sent this message
#
# Who's online for the web client: set VME_WHO_JSON to a file the web server
# serves, e.g. VME_WHO_JSON=/www/valhalla/v3/live/who.json, and every who list
# the MUD sends to Discord is also written there as
#     {"updated": <unix time>, "players": [{"name": "Papi", "tags": ["Admin", "Builder"]}]}
# Unset, nothing is written.
#
WHO_JSON = os.environ.get('VME_WHO_JSON', '')

class MyDispatcher():
    pipeDiscord = None

    def __init__(self):
        self.pipeDiscord = self.OpenPipeWONB('pipeDiscord')
        # add more pipes here

    # write-only-non-blocking
    def OpenPipeWONB(self, fifoname):
        print('Opening connection to named pipe: ' + fifoname)

        try:
            os.mkfifo(fifoname, 0o333)
        except OSError as oe:
            if oe.errno != errno.EEXIST:
                print('Cannot open the pipe '+fifoname)
                raise
            else:
                print('Reusing pipe '+fifoname)

        print('Pipe has been accessed ' + fifoname)

        tmppipe = None
        while (tmppipe == None):
            try:
                tmppipe = os.open(fifoname, os.O_WRONLY | os.O_NONBLOCK)
            except OSError as ex:
                if ex.errno == errno.ENXIO:
                    print("Nobody is listening to " + fifoname + ". Pausing and retrying")
                else:
                    print("Exception", ex)
                time.sleep(1)

        print('Pipe has been opened as write-only non-blocking: ' + fifoname)
        return tmppipe


    def DispatchDiscord(self, message):
        # Convert some HTML bits
        message = message.replace("&quot;", '"')
        message = message.replace("&amp;", '&')
        message = message.replace("&lt;", '<')
        message = message.replace("&gt;", '>')
        print('Relaying message to Discord: ' + message)
        encmsg = message.encode()
        try:
            numBytes = os.write(self.pipeDiscord, encmsg)
            print('Message of ', len(encmsg), ' bytes sent, wrote ', numBytes, ' bytes')
        except OSError as e:
            if e.errno == errno.EAGAIN:
                print('Warning: Discord pipe buffer full, message dropped')
            else:
                print('Error writing to Discord pipe:', e)
        except Exception as e:
            print('Unexpected error writing to Discord pipe:', e)


    # The MUD's who list: "Papi (Admin) * Builder *&&Dwili &&", or a
    # "No (visible) players..." line when nobody is on.
    def WriteWho(self, line):
        if not WHO_JSON:
            return
        players = []
        for entry in line.split('&&'):
            entry = entry.strip()
            if not entry or entry.startswith('No (visible) players'):
                continue
            name, _, rest = entry.partition(' ')
            tags = [a or b for a, b in re.findall(r'\(([^)]+)\)|\* ([^*]+?) \*', rest)]
            players.append({'name': name, 'tags': tags})
        tmp = WHO_JSON + '.tmp'
        try:
            with open(tmp, 'w') as f:
                json.dump({'updated': int(time.time()), 'players': players}, f)
            os.chmod(tmp, 0o644)  # the web server reads it; this script runs with umask 007
            os.replace(tmp, WHO_JSON)
        except OSError as e:
            print('Cannot write ' + WHO_JSON + ':', e)


    #
    def ProcessMUDMessage(self, txt):
        try:
            print('MUD said: [' + txt +']')
            sDispatch = txt.partition(' ')[0]
            
            if (sDispatch == "discord"):
                rest = txt.split(' ', 1)[1]
                if rest.startswith('who '):
                    self.WriteWho(rest[4:].rstrip('\n'))
                self.DispatchDiscord("<discord> " + rest)
            else:
                print('Unrecognized destination:' + sDispatch)
        except Exception as e:
            print('Error processing MUD message:', e)
            import traceback
            traceback.print_exc()


    #
    # Listens to the named PIPE MUDfifo for chat channel from dispatcher
    #
    def MUDlistener(self):
        FIFO = 'pipeDispatcher'
        print('Opening blocking read connection to MUD : ' + FIFO)
        try:
            os.mkfifo(FIFO, 0o666)
        except OSError as oe:
            if oe.errno != errno.EEXIST:
                print('Cannot open the pipe '+FIFO)
                raise
            else:
                print('Reusing pipe '+FIFO)

        print('Pipe has been accessed ' + FIFO)
        while True:
            try:
                with open(FIFO) as fifopipe:
                    for line in fifopipe:
                        try:
                            self.ProcessMUDMessage(line)
                            print('Ready for next line in fifopipe')
                        except Exception as e:
                            print('Error processing line from pipe:', e)
                            import traceback
                            traceback.print_exc()
                            continue
            except Exception as e:
                print('Error with pipe connection, reopening:', e)
                import traceback
                traceback.print_exc()
                time.sleep(5)  # Wait before retrying
                continue

dispatcher = MyDispatcher()
MyDispatcher.MUDlistener(dispatcher)
