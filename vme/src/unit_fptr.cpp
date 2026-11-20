#include "unit_fptr.h"

#include "constants.h"
#include "dbfind.h"
#include "dil.h"
#include "formatter.h"
#include "json_helper.h"
#include "spec_assign.h"
#include "utility.h"

#include <cstdlib>

unit_fptr::unit_fptr()
{
    index = 0;
    priority = FN_PRI_CHORES;
    heart_beat = PULSE_SEC * 5;
    flags = 0;
    data = nullptr;
    next = nullptr;
    event = nullptr;
}

unit_fptr::~unit_fptr()
{
    data = nullptr;
}

int unit_fptr::destruct_classindex()
{
    return DR_FUNC;
}

ubit16 unit_fptr::getFunctionPointerIndex() const
{
    return index;
}

void unit_fptr::readFunctionPointerIndexFrom(CByteBuffer &buf, int &nError)
{
    index = buf.ReadU16(&nError);
}

void unit_fptr::setFunctionPointerIndex(ubit16 value)
{
    index = value;
}

ubit8 unit_fptr::getFunctionPriority() const
{
    return priority;
}

void unit_fptr::readFunctionPriorityFrom(CByteBuffer &buf, int &nError)
{
    priority = buf.ReadU8(&nError);
}

void unit_fptr::setFunctionPriority(ubit8 value)
{
    priority = value;
}

ubit16 unit_fptr::getHeartBeat() const
{
    return heart_beat;
}

ubit16 *unit_fptr::getHeartBeatPtr()
{
    return &heart_beat;
}

void unit_fptr::readHeartBeatFrom(CByteBuffer &buf, int &nError)
{
    heart_beat = buf.ReadU16(&nError);
}

void unit_fptr::setHeartBeat(ubit16 value)
{
    heart_beat = value;
}

bool unit_fptr::isActivateOnEventFlagSet(ubit16 value)
{
    return flags & value;
}

void unit_fptr::setActivateOnEventFlag(ubit16 value)
{
    flags |= value;
}

void unit_fptr::setAllActivateOnEventFlags(ubit16 value)
{
    flags = value;
}

void unit_fptr::removeActivateOnEventFlag(ubit16 value)
{
    flags &= ~value;
}

void unit_fptr::readActivateOnEventFlagsFrom(CByteBuffer &buf, int &nError)
{
    flags = buf.ReadU16(&nError);
}

ubit16 unit_fptr::getAllActivateOnEventFlags() const
{
    return flags;
}

void *unit_fptr::getData()
{
    return data;
}

void unit_fptr::freeData()
{
    FREE(data);
}

void unit_fptr::setData(void *value)
{
    data = value;
}

void unit_fptr::readDataFrom(CByteBuffer &buf)
{
    buf.ReadStringAlloc((char **)&data);
}

unit_fptr *unit_fptr::getNext()
{
    return next;
}

void unit_fptr::setNext(unit_fptr *value)
{
    next = value;
}

eventq_elem *unit_fptr::getEventQueue()
{
    return event;
}

void unit_fptr::setEventQueue(eventq_elem *value)
{
    event = value;
}

namespace
{
void write_dil_value(const dilvar &var, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    switch (var.type)
    {
        case DILV_INT:
            writer.Int(var.val.integer);
            break;
        case DILV_SP:
            json::write_char_pointer(var.val.string, writer);
            break;
        case DILV_SLP:
            writer.StartArray();
            for (ubit32 i = 0; var.val.namelist && i < var.val.namelist->Length(); i++)
            {
                writer.String(var.val.namelist->Name(i));
            }
            writer.EndArray();
            break;
        case DILV_ILP:
            writer.StartArray();
            for (sbit32 i = 0; var.val.intlist && i < var.val.intlist->Length(); i++)
            {
                writer.Int(var.val.intlist->Value(i));
            }
            writer.EndArray();
            break;
        case DILV_UP:
            json::write_unit_id(var.val.unitptr, writer);
            break;
        default:
            writer.Null();
            break;
    }
}

/// An argument of a dilcopy that hasn't run yet, as dil_direct_init reads it
void write_dilcopy_arg(const dilargtype &arg, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    switch (arg.type)
    {
        case DILV_INT:
            writer.Int(static_cast<int>(arg.data.num));
            break;
        case DILV_SP:
            json::write_char_pointer(arg.data.string, writer);
            break;
        case DILV_SLP:
            writer.StartArray();
            for (char **sl = arg.data.stringlist; sl && *sl; sl++)
            {
                writer.String(*sl);
            }
            writer.EndArray();
            break;
        case DILV_ILP:
            writer.StartArray();
            for (int m = 1; arg.data.intlist && m <= arg.data.intlist[0]; m++)
            {
                writer.Int(arg.data.intlist[m]);
            }
            writer.EndArray();
            break;
        default:
            writer.Null();
            break;
    }
}

std::string template_name(const diltemplate *tmpl)
{
    return tmpl->zone ? diku::format_to_str("%s@%s", tmpl->prgname, tmpl->zone->getName()) : tmpl->prgname;
}
} // namespace

/**
 * The program: "name" is the special function's, or the DIL template's (name@zone), and "args" a
 * DIL program's arguments: as the zone file gives them while it is a dilcopy that hasn't started,
 * or as its variables hold them once it runs (a program may have changed them).
 */
void unit_fptr::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        if (index == SFUN_DIL_INTERNAL && data && static_cast<const dilprg *>(data)->frame)
        {
            const dilframe &frame = static_cast<const dilprg *>(data)->frame[0];
            json::write_kvp("name", template_name(frame.tmpl), writer);
            writer.String("args");
            writer.StartArray();
            for (int i = 0; i < frame.tmpl->argc && i < frame.tmpl->varc; i++)
            {
                write_dil_value(frame.vars[i], writer);
            }
            writer.EndArray();
        }
        else if (index == SFUN_DILCOPY_INTERNAL && data)
        {
            const auto *dilargs = static_cast<const dilargstype *>(data);
            const diltemplate *tmpl = find_dil_template(dilargs->name);
            json::write_kvp("name", tmpl ? template_name(tmpl) : std::string(dilargs->name), writer);
            writer.String("args");
            writer.StartArray();
            for (int i = 0; i < dilargs->no; i++)
            {
                write_dilcopy_arg(dilargs->dilarg[i], writer);
            }
            writer.EndArray();
        }
        else
        {
            json::write_kvp("name", g_unit_function_array[index].name, writer);
        }
        json::write_kvp("priority", priority, writer);
        json::write_kvp("heart_beat", heart_beat, writer);
        std::string bits;
        json::write_kvp("flags", sprintbit(bits, flags, g_sfb_flags), writer);
    }
    writer.EndObject();
}
