#pragma once
#include "dil.h"

#include <rapidjson/prettywriter.h>

/**
 * Helper functions for writing out JSON key/value pairs
 */
namespace json
{
void write_pointer_kvp(const char *name, const void *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_pointer(const void *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_char_pointer_kvp(const char *name, const char *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_char_pointer(const char *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

template<typename Object>
void write_object_pointer_kvp(const char *name, const Object *ptr, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    if (ptr)
    {
        ptr->toJSON(writer);
    }
    else
    {
        writer.Null();
    }
}

template<typename Object>
void write_object_value_kvp(const char *name, const Object &obj, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.String(name);
    obj.toJSON(writer);
}

void write_dil_var_type(DilVarType_e vartpe, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_unit_id_kvp(const char *name, const unit_data *unit, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_unit_id(const unit_data *unit, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_zone_id(const char *name, const zone_type *zone, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_kvp(const char *name, ubit64 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, ubit32 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, ubit16 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, ubit8 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_kvp(const char *name, sbit64 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, sbit32 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, sbit16 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, sbit8 value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_kvp(const char *name, bool value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_kvp(const char *name, const std::string &value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
void write_kvp(const char *name, const char *value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

void write_kvp(const char *name, double value, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

} // namespace json