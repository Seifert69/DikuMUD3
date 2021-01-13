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
        print('Relaying message to Discord: ' + message)
        os.write(self.pipeDiscord, message.encode())
        print('Message Sent')


    #
    def ProcessMUDMessage(self, txt):
        print('MUD said:' + txt)
        sDispatch = txt.partition(' ')[0]
        
        if (sDispatch == "discord"):
            self.DispatchDiscord("<discord> " + txt.split(' ', 1)[1])
        else:
            print('Unrecognized destination:' + sDispatch)


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
            with open(FIFO) as fifopipe:
                for line in fifopipe:
                    self.ProcessMUDMessage(line)

dispatcher = MyDispatcher()
MyDispatcher.MUDlistener(dispatcher)
