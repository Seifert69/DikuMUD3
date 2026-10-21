import threading
import os
import errno
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


    #
    def ProcessMUDMessage(self, txt):
        try:
            print('MUD said: [' + txt +']')
            sDispatch = txt.partition(' ')[0]
            
            if (sDispatch == "discord"):
                self.DispatchDiscord("<discord> " + txt.split(' ', 1)[1])
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
