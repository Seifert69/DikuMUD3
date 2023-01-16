#include "json_helper.h"

#include "unit_data.h"

#include <sstream>

namespace json
{
void write_pointer_kvp(const char *name, const void *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    write_pointer(ptr, writer);
}

void write_pointer(const void *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    static std::ostringstream strm;
    if (ptr)
    {
        strm.str("");
        strm << ptr;
        writer.String(strm.str().c_str());
    }
    else
    {
        writer.Null();
    }
}

void write_char_pointer_kvp(const char *name, const char *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    write_char_pointer(ptr, writer);
}

void write_char_pointer(const char *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    if (ptr)
    {
        writer.String(ptr);
    }
    else
    {
        writer.Null();
    }
}

void write_dil_var_type(DilVarType_e vartpe, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    switch (vartpe)
    {
        case DILV_INVALID:
            writer.String("invalid");
            break;
        case DILV_UP:
            writer.String("up");
            break;
        case DILV_SP:
            writer.String("sp");
            break;
        case DILV_SLP:
            writer.String("slp");
            break;
        case DILV_EDP:
            writer.String("edp");
            break;
        case DILV_INT:
            writer.String("int");
            break;
        case DILV_UPR:
            writer.String("upr");
            break;
        case DILV_SPR:
            writer.String("spr");
            break;
        case DILV_SLPR:
            writer.String("slpr");
            break;
        case DILV_EDPR:
            writer.String("edpr");
            break;
        case DILV_SINT1R:
            writer.String("sint1r");
            break;
        case DILV_SINT2R:
            writer.String("sint2r");
            break;
        case DILV_SINT4R:
            writer.String("sint4r");
            break;
        case DILV_UINT1R:
            writer.String("uint1r");
            break;
        case DILV_UINT2R:
            writer.String("uint2r");
            break;
        case DILV_UINT4R:
            writer.String("uint4r");
            break;
        case DILV_ERR:
            writer.String("err");
            break;
        case DILV_NULL:
            writer.String("null");
            break;
        case DILV_FAIL:
            writer.String("fail");
            break;
        case DILV_UEDP:
            writer.String("uedp");
            break;
        case DILV_FUNC:
            writer.String("func");
            break;
        case DILV_PROC:
            writer.String("proc");
            break;
        case DILV_HASHSTR:
            writer.String("hashstr");
            break;
        case DILV_ZP:
            writer.String("zp");
            break;
        case DILV_ZPR:
            writer.String("zpr");
            break;
        case DILV_UZP:
            writer.String("uzp");
            break;
        case DILV_EZP:
            writer.String("ezp");
            break;
        case DILV_UEZP:
            writer.String("uezp");
            break;
        case DILV_SP_SLP:
            writer.String("sp_slp");
            break;
        case DILV_CP:
            writer.String("cp");
            break;
        case DILV_CPR:
            writer.String("cpr");
            break;
        case DILV_UEZCP:
            writer.String("uezcp");
            break;
        case DILV_UCP:
            writer.String("ucp");
            break;
        case DILV_ESLP:
            writer.String("eslp");
            break;
        case DILV_ILP:
            writer.String("ilp");
            break;
        case DILV_ILPR:
            writer.String("ilpr");
            break;
        case DILV_ESLIP:
            writer.String("eslip");
            break;
        case DILV_ALL:
            writer.String("all");
            break;
    }
}

void write_unit_id_kvp(const char *name, const unit_data *unit, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    write_unit_id(unit, writer);
}

void write_unit_id(const unit_data *unit, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    if (unit)
    {
        writer.String(unit->getID().c_str());
    }
    else
    {
        writer.Null();
    }
}

void write_zone_id(const char *name, const zone_type *zone, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    if (zone)
    {
        writer.String(zone->getName().c_str());
    }
    else
    {
        writer.Null();
    }
}

void write_kvp(const char *name, ubit64 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Uint64(value);
}

void write_kvp(const char *name, ubit32 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Uint(value);
}

void write_kvp(const char *name, ubit16 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Uint(value);
}

void write_kvp(const char *name, ubit8 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Uint(value);
}

void write_kvp(const char *name, sbit64 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Int64(value);
}

void write_kvp(const char *name, sbit32 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Int(value);
}

void write_kvp(const char *name, sbit16 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Int(value);
}

void write_kvp(const char *name, sbit8 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Int(value);
}

void write_kvp(const char *name, bool value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Bool(value);
}

void write_kvp(const char *name, const std::string &value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.String(value.c_str());
}

void write_kvp(const char *name, const char *value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    if (value)
    {
        writer.String(value);
    }
    else
    {
        writer.Null();
    }
}

void write_kvp(const char *name, double value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    writer.Double(value);
}

} // namespace json
