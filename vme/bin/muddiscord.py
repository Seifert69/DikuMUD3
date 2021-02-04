import discord
import os
import errno
import time
import asyncio
import sys



# See dispatcher.py for overall architecture
#
# https://discordpy.readthedocs.io/en/latest/intro.html
# 
# python3 -m pip install -U discord.py

class MyClient(discord.Client):
    pipeMUD = None
    pipeDiscord = None

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.pipeMUD = self.OpenPipeWONB('pipeMUD')
        self.pipeDiscord = self.OpenPipeRONB('pipeDiscord')

        # This is not a thread, it let's the loop of the library
        # call this function. So we can't use blocking in the listener
        self.bg_task = self.loop.create_task(self.pipelistener())


    def OpenPipeRONB(self, fifoname):
        print('Opening RO connection to named pipe: ' + fifoname)
        try:
            os.mkfifo(fifoname, 0o555)
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
                tmppipe = os.open(fifoname, os.O_RDONLY | os.O_NONBLOCK)
            except OSError as ex:
                if ex.errno == errno.ENXIO:
                    print("Nobody is listening to " + fifoname + ". Pausing and retrying")
                else:
                    print("Exception", ex)
                time.sleep(1)

        print('Pipe has been opened: ' + fifoname)
        return tmppipe


    # write-only-non-blocking
    def OpenPipeWONB(self, fifoname):
        print('Opening named pipe WR ONLY: ' + fifoname)

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

        print('Pipe has been opened: ' + fifoname)
        return tmppipe


    #
    # When logged successfully into Discord
    #
    async def on_ready(self):
        #print('We have logged in as {0.user}'.format(client))
        print('Logged in to Discord as: ',  self.user.name, self.user.id)
        print('------')

        #
        # When I create this thread, it seems that it stops listening to messages 
        #
        #print("I'm still running, ya ya ya ")


    #
    # When receiving a chat from discord
    #
    async def on_message(self, message):
        if message.author == self.user:
            return

        print("Message from Discord to the MUD-> ", message.content)

        # write the message received from Discord to pipeMUD so that
        # the MUD server receives it.
        #
        str = "discord " + message.channel.name + " @ @" + message.author.name + " says, '" + message.content + "'"
        os.write(self.pipeMUD, str.encode())
        #if message.content.startswith('$hello'):
        #    await message.channel.send('da robot replies Hello!')

    allChannels = {}
    
    def resolveChannel(self, channelname):
        print('Resolving channel name = ', channelname)
        if channelname in self.allChannels:
            return self.allChannels[channelname] 

        channel = discord.utils.get(self.get_all_channels(), name=channelname)
        if channel == None:
            if "mud" in self.allChannels:
                channel = self.allChannels['mud']
        else:
            self.allChannels[channelname] = channel
            print('Resolved new channel ', channelname)

        return channel

    #
    # Listens to the named pipeDiscord for chat channel from MUD dispatcher
    #
    async def pipelistener(self):
        await self.wait_until_ready()

        print('pipeLIstener running')

        if self.resolveChannel('mud') == None:
            print('Cant find the default channel (MUD).')
            sys.exit(1)

        if self.resolveChannel('mudstatus') == None:
            print('Cant find the mudstatus channel.')
            sys.exit(1)

        if self.resolveChannel('builder') == None:
            print('Cant find the builder channel.')

        print("Getting my whomsg to pin and alter")
        whomsg = await self.resolveChannel('mudstatus').fetch_message(802048971446157332)
        if whomsg == None:
            print('Cant find the mudstatus channel message with ID 802048971446157332. Make one and change the ID')
            sys.exit(1)

        print("Got whomsg and ready to begin")
        # WORKS whomsg = discord.utils.get(await self.resolveChannel('mudstatus').history(limit=100).flatten())
        # print("Message has this content: ", whomsg.content)
        # WORKS await whomsg.edit(content="I just modified this message content")

        while not self.is_closed():
            grace = True
            result = []

            #await channel2.send("Quick test")
            #sys.exit(0)

            while (True):
                try:
                    line = os.read(self.pipeDiscord, 1000)
                    if not line:
                        break
                    else:
                        content = line.decode("utf-8")
                        print('Debug: Got line:', content)
                        line = content.split("\n")
                        result.extend(line)
                except OSError as e:
                    if e.errno == 11:
                        if grace:
                            # grace period, first write to pipe might take some time
                            # further reads after opening the file are then successful
                            grace = False
                            await asyncio.sleep(1) # task runs every 1 seconds
                        else:
                            break
                    else:
                        print(e)
                        break

            if (result != []):
                for line in result:
                    if (line) and (line.strip()):
                        words = line.split(" ")
                        channel = None
                        print("Line = ", line)

                        if words[2][0] == "#":
                            print('words[2][1:] = ', words[2][1:])
                            channel = self.resolveChannel(words[2][1:])
                        else:
                            channel = self.resolveChannel("mud")

                        if channel == None:
                            print('Impossible channel')
                            continue

                        if words[1] == "who":
                            line = line[14:]  # remove "<discord> who " 
                            line = line.replace('&&', '\n')
                            print('WHO update:\n', line)
                            await whomsg.edit(content=line)
                        elif words[1] == "msg":
                            print('Msg Line = ', line)
                            print('Partition = ', line.split(' ', 3)[-1])
                            await channel.send(line.split(' ', 3)[-1])
                        else:
                            print("Unknown type ", words[1])
                            continue

            await asyncio.sleep(1) # task runs every 1 seconds

        print('Pipe listener end')


token = open('discord.token', 'r').read()
if token == "":
    print('Discord Bot Token could not be loaded from dicord.token')
else:
    print('Discord Bot Token loaded, logging in')
    client = MyClient()
    client.run(token)
    print("Run has ended")
