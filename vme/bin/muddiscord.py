import discord
import os
import errno
import time
import asyncio



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
        str = "discord <discord> #" + message.channel.name + " @" + message.author.name + " says, '" + message.content + "'"
        os.write(self.pipeMUD, str.encode())
        #if message.content.startswith('$hello'):
        #    await message.channel.send('da robot replies Hello!')


    #
    # Listens to the named pipeDiscord for chat channel from MUD dispatcher
    #
    async def pipelistener(self):
        await self.wait_until_ready()

        while not self.is_closed():
            grace = True
            result = []
            channel = self.get_channel(798026631620067388) # mud channel

            while (True):
                try:
                    line = os.read(self.pipeDiscord, 200)
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
                    if (line):
                        print('Line = ', line)
                        await channel.send(line)
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