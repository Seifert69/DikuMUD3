#include "descriptor_data.h"

#include "account.h"
#include "comm.h"
#include "db.h"
#include "formatter.h"
#include "main_functions.h"
#include "nanny.h"
#include "slog.h"
#include "system.h"
#include "utils.h"

descriptor_data::descriptor_data(cMultiHook *pe)
{
    static int nid = 0;

    g_no_connections++;
    if (g_no_connections > g_max_no_connections)
    {
        g_max_no_connections = g_no_connections;
    }

    /* init desc data */
    multi = pe;
    id = ++nid;

    state = 0;
    fptr = nanny_get_name;
    wait = 1;
    timer = 0;
    prompt_mode = PROMPT_SENT;
    *last_cmd = *history = '\0';

    host[0] = 0;
    nPort = 0;
    nLine = 255;

    localstr = nullptr;
    postedit = nullptr;
    editing = nullptr;
    editref = nullptr;

    original = nullptr;
    snoop.setSnooping(nullptr);
    snoop.setSnoopBy(nullptr);
    replyid = (ubit32)-1;

    /* Make a new PC struct */
    character = new EMPLACE(pc_data) pc_data;
    init_char(character);
    UCHAR(character)->setDescriptor(this);

    /* prepend to list */
    next = g_descriptor_list;
    g_descriptor_list = this;
}

void descriptor_data::RemoveBBS()
{
    if (nLine != 255)
    {
        auto filename = diku::format_to_str("%s%d.%d", BBS_DIR, nPort, nLine);
        remove(filename.c_str());
    }
}

void descriptor_data::CreateBBS()
{
    if (nLine != 255)
    {
        auto filename = diku::format_to_str("%s%d.%d", BBS_DIR, nPort, nLine);

        if (!character)
        {
            slog(LOG_ALL, 0, "No character in %s.", filename);
            return;
        }

        FILE *f = fopen(filename.c_str(), "wb");

        if (!f)
        {
            slog(LOG_ALL, 0, "Could not create %s.", filename);
            return;
        }

        if (account_is_overdue(this->character))
        {
            fprintf(f, "1\n");
        }
        else
        {
            fprintf(f, "0\n");
        }

        fprintf(f, "%d\n", PC_ACCOUNT(this->character).getTotalCredit());
        fprintf(f, "%s\n", PC_FILENAME(this->character));
        fprintf(f, "%d\n", CHAR_LEVEL(this->character));
        fprintf(f, "%d\n", PC_ACCESS_LEVEL(this->character));

        fclose(f);
    }
}

descriptor_data::~descriptor_data()
{
    RemoveBBS();
    nLine = 255;
}

time_t descriptor_data::getLastLogonTime() const
{
    return logon;
}

void descriptor_data::setLastLogonTime(time_t value)
{
    logon = value;
}

cMultiHook *descriptor_data::getMultiHookPtr()
{
    return multi;
}

ubit16 descriptor_data::getMultiHookID() const
{
    return id;
}

const descriptor_data::FunctionPtr descriptor_data::getFunctionPtr() const
{
    return fptr;
}

void descriptor_data::callFunctionPtr(descriptor_data *data, char *arg) const
{
    fptr(data, arg);
}

void descriptor_data::setFunctionPtr(descriptor_data::FunctionPtr value)
{
    fptr = value;
}

int descriptor_data::getState() const
{
    return state;
}

void descriptor_data::setState(int value)
{
    state = value;
}
int descriptor_data::postincrementState()
{
    return state++;
}

const char *descriptor_data::getHostname() const
{
    return host;
}

void descriptor_data::setHostname(const char *value)
{
    strncpy(host, value, sizeof(host));
    host[sizeof(host) - 1] = 0;
}

ubit16 descriptor_data::getMplexPortNum() const
{
    return nPort;
}

void descriptor_data::setMplexPortNum(ubit16 value)
{
    nPort = value;
}

ubit8 descriptor_data::getSerialLine() const
{
    return nLine;
}

void descriptor_data::setSerialLine(ubit8 value)
{
    nLine = value;
}

int descriptor_data::predecrementLoopWaitCounter()
{
    return --wait;
}

void descriptor_data::setLoopWaitCounter(int value)
{
    wait = value;
}

ubit16 descriptor_data::getMinutesPlayerIdle() const
{
    return timer;
}

void descriptor_data::setMinutesPlayerIdle(ubit16 value)
{
    timer = value;
}

void descriptor_data::incrementMinutesPlayerIdle()
{
    ++timer;
}

void descriptor_data::clearLocalString()
{
    FREE(localstr);
    localstr = nullptr;
}

const char *descriptor_data::getLocalString() const
{
    return localstr;
}

void descriptor_data::setLocalString(const char *str)
{
    if (localstr == nullptr)
    {
        CREATE(localstr, char, strlen(str) + 8);
        strcpy(localstr, str);
    }
    else
    {
        RECREATE(localstr, char, strlen(localstr) + strlen(str) + 8);
        strcat(localstr, str);
    }
}

void descriptor_data::appendLocalString(const char *str)
{
    strcat(localstr, str);
}

bool descriptor_data::hasPostEditFunctionPtr() const
{
    return postedit != nullptr;
}

void descriptor_data::callPostEditFunctionPtr(descriptor_data *data) const
{
    postedit(data);
}

void descriptor_data::setPostEditFunctionPtr(PostEditFunctionPtr value)
{
    postedit = value;
}

const unit_data *descriptor_data::cgetEditing() const
{
    return editing;
}

unit_data *descriptor_data::getEditing()
{
    return editing;
}

void descriptor_data::setEditing(unit_data *value)
{
    editing = value;
}

void descriptor_data::setEditReference(void *value)
{
    editref = value;
}

const void *descriptor_data::getEditingReference() const
{
    return editref;
}

int descriptor_data::getPromptMode() const
{
    return prompt_mode;
}

void descriptor_data::setPromptMode(int value)
{
    prompt_mode = value;
}

const char *descriptor_data::getLastCommand() const
{
    return last_cmd;
}

void descriptor_data::setLastCommand(const char *value)
{
    strncpy(last_cmd, value, sizeof(last_cmd) - 1);
    last_cmd[sizeof(last_cmd) - 1] = '\0';
}

char *descriptor_data::getCommandHistory()
{
    return history;
}

void descriptor_data::setCommandHistory(const char *value)
{
    strncpy(history, value, sizeof(history) - 1);
    history[sizeof(history) - 1] = '\0';
}

cQueue &descriptor_data::getInputQueue()
{
    return qInput;
}

const unit_data *descriptor_data::cgetCharacter() const
{
    return character;
}

unit_data *descriptor_data::getCharacter()
{
    return character;
}

void descriptor_data::setCharacter(unit_data *value)
{
    character = value;
}

const unit_data *descriptor_data::cgetOriginalCharacter() const
{
    return original;
}

unit_data *descriptor_data::getOriginalCharacter()
{
    return original;
}

void descriptor_data::setOriginalCharacter(unit_data *value)
{
    original = value;
}

const snoop_data &descriptor_data::cgetSnoopData() const
{
    return snoop;
}

snoop_data &descriptor_data::getSnoopData()
{
    return snoop;
}

descriptor_data *descriptor_data::getNext()
{
    return next;
}

void descriptor_data::setNext(descriptor_data *value)
{
    next = value;
}
