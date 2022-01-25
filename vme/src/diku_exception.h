#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>

/**
 * Creates an exception that the operator<< can be used on at throw site
 *
 * eg
 * throw diku_exception() << "My message: [" << my_var << "] more text here:
 *
 * You can pass the following to the ctor for location information
 * FFL  Filename, Function, Line Number
 * FPFL Filename, Pretty Function, Line Number
 * FL   Filename, Line Number
 * PFL  Pretty Function, Line Number
 *
 */
class diku_exception : public std::runtime_error
{
public:
    /**
     * Default Constructor
     */
    diku_exception()
        : std::runtime_error("")
        , m_stream(std::make_shared<std::stringstream>())
        , m_message()
        , m_filename()
        , m_function()
        , m_line_number()
    {
    }
    /**
     * Location aware constructor. Use with 'FFL' and 'FPFL' macros
     * @param filename Filename of source file
     * @param function Function name created in
     * @param line_number Source code line number
     */
    diku_exception(std::string filename, std::string function, int line_number)
        : std::runtime_error("")
        , m_stream(std::make_shared<std::stringstream>())
        , m_message()
        , m_filename(std::filesystem::path(std::move(filename)).filename())
        , m_function(std::move(function))
        , m_line_number(line_number)
    {
        // __func__ is boringly undecorated
        if (!m_function->empty() && (*m_function)[m_function->length() - 1] != ')')
        {
            (*m_function) += "()";
        }
    }
    /**
     * Location aware constructor. Use with 'FL' and 'PFL' macros
     * @param function Function name created in
     * @param line_number Source code line number
     */
    diku_exception(std::string function, int line_number)
        : std::runtime_error("")
        , m_stream(std::make_shared<std::stringstream>())
        , m_message()
        , m_filename()
        , m_function(std::move(function))
        , m_line_number(line_number)
    {
        // __func__ is boringly undecorated
        if (!m_function->empty() && (*m_function)[m_function->length() - 1] != ')')
        {
            (*m_function) += "()";
        }
    }
    diku_exception(const diku_exception &) = default;
    diku_exception(diku_exception &&) = default;
    diku_exception &operator=(const diku_exception &) = default;
    diku_exception &operator=(diku_exception &&) = default;
    ~diku_exception() noexcept override = default;

    /**
     * Streaming operator
     * @tparam T Type of value
     * @param value Value to add to message
     * @return Reference to this
     */
    template<typename T>
    auto operator<<(const T &value) noexcept -> diku_exception &
    {
        (*m_stream) << value;
        return *this;
    }

    /**
     * @return Message stored in the exception
     */
    auto what() const noexcept -> const char * override
    {
        m_message.clear();
        std::ostringstream location;
        if (m_filename && m_function && m_line_number)
        {
            location << m_filename.value() << ':' << m_line_number.value() << ' ' << m_function.value() << ' ';
        }
        else if (m_function && m_line_number)
        {
            location << m_function.value() << ':' << m_line_number.value() << ' ';
        }
        auto loc_string = location.str();
        if (!loc_string.empty())
        {
            m_message = loc_string;
        }
        m_message += m_stream->str();
        return m_message.c_str();
    }

    /**
     * Helper functions
     */
    auto filename() const noexcept -> std::string { return m_filename.value_or("not set"); }
    auto function() const noexcept -> std::string { return m_function.value_or("not set"); }
    auto line_number() const noexcept -> int { return m_line_number.value_or(-1); }

private:
    // Stream construction is expensive to use pointer to avoid unneeded ctor/dtor calls
    // as the exception is propagated
    mutable std::shared_ptr<std::stringstream> m_stream; // Pointer to stream for messages
    mutable std::string m_message;
    std::optional<std::string> m_filename;
    std::optional<std::string> m_function;
    std::optional<int> m_line_number;
};

// clang-format off
#define FFL  __FILE__,__func__,__LINE__
#define FPFL __FILE__,__PRETTY_FUNCTION__,__LINE__
#define FL   __func__,__LINE__
#define PFL  __PRETTY_FUNCTION__,__LINE__
// clang-format on
