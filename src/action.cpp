#include "action.h"
#include "string.h"

#include <cstring>
#include <cwchar>

Action::Action() : m_readPos(0), m_sendPos(0), m_isValid(true)
{
}

Action::~Action() = default;

Action::Action(const Action&) = default;

Action& Action::operator=(const Action&) = default;

Action::Action(Action&&) noexcept = default;

Action& Action::operator=(Action&&) noexcept = default;

void Action::append(const void* data, std::size_t sizeInBytes)
{
    if (data && (sizeInBytes > 0))
    {
        std::size_t start = m_data.size();
        m_data.resize(start + sizeInBytes);
        std::memcpy(&m_data[start], data, sizeInBytes);
    }
}

void Action::clear()
{
    m_data.clear();
    m_readPos = 0;
    m_isValid = true;
}

Action& Action::operator>>(std::int8_t& data)
{
    if (checkSize(sizeof(data)))
    {
        std::memcpy(&data, &m_data[m_readPos], sizeof(data));
        m_readPos += sizeof(data);
    }

    return *this;
}

Action& Action::operator>>(std::uint8_t& data)
{
    if (checkSize(sizeof(data)))
    {
        std::memcpy(&data, &m_data[m_readPos], sizeof(data));
        m_readPos += sizeof(data);
    }

    return *this;
}

Action& Action::operator>>(std::int32_t& data)
{
    if (checkSize(sizeof(data)))
    {
        std::memcpy(&data, &m_data[m_readPos], sizeof(data));
        data = static_cast<std::int32_t>(ntohl(static_cast<uint32_t>(data)));
        m_readPos += sizeof(data);
    }

    return *this;
}

Action& Action::operator>>(std::uint32_t& data)
{
    if (checkSize(sizeof(data)))
    {
        std::memcpy(&data, &m_data[m_readPos], sizeof(data));
        data = ntohl(data);
        m_readPos += sizeof(data);
    }

    return *this;
}

Action& Action::operator>>(char* data)
{
    // First extract string length
    std::uint32_t length = 0;
    *this >> length;

    if ((length > 0) && checkSize(length))
    {
        // Then extract characters
        std::memcpy(data, &m_data[m_readPos], length);
        data[length] = '\0';

        // Update reading position
        m_readPos += length;
    }

    return *this;
}

Action& Action::operator>>(std::string& data)
{
    // First extract string length
    std::uint32_t length = 0;
    *this >> length;

    data.clear();
    if ((length > 0) && checkSize(length))
    {
        // Then extract characters
        data.assign(&m_data[m_readPos], length);

        // Update reading position
        m_readPos += length;
    }

    return *this;
}

Action& Action::operator>>(wchar_t* data)
{
    // First extract string length
    std::uint32_t length = 0;
    *this >> length;

    if ((length > 0) && checkSize(length * sizeof(std::uint32_t)))
    {
        // Then extract characters
        for (std::uint32_t i = 0; i < length; ++i)
        {
            std::uint32_t character = 0;
            *this >> character;
            data[i] = static_cast<wchar_t>(character);
        }
        data[length] = L'\0';
    }

    return *this;
}

Action& Action::operator>>(std::wstring& data)
{
    // First extract string length
    std::uint32_t length = 0;
    *this >> length;

    data.clear();
    if ((length > 0) && checkSize(length * sizeof(std::uint32_t)))
    {
        // Then extract characters
        for (std::uint32_t i = 0; i < length; ++i)
        {
            std::uint32_t character = 0;
            *this >> character;
            data += static_cast<wchar_t>(character);
        }
    }

    return *this;
}

Action& Action::operator<<(std::int8_t data)
{
    append(&data, sizeof(data));
    return *this;
}

Action& Action::operator<<(std::uint8_t data)
{
    append(&data, sizeof(data));
    return *this;
}

Action& Action::operator<<(std::int32_t data)
{
    std::int32_t toWrite = static_cast<std::int32_t>(htonl(static_cast<uint32_t>(data)));
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Action& Action::operator<<(std::uint32_t data)
{
    std::uint32_t toWrite = htonl(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}

Action& Action::operator<<(const char* data)
{
    // First insert string length
    auto length = static_cast<std::uint32_t>(std::strlen(data));
    *this << length;

    // Then insert characters
    append(data, length * sizeof(char));

    return *this;
}

Action& Action::operator<<(const std::string& data)
{
    // First insert string length
    auto length = static_cast<std::uint32_t>(data.size());
    *this << length;

    // Then insert characters
    if (length > 0)
        append(data.c_str(), length * sizeof(std::string::value_type));

    return *this;
}

Action& Action::operator<<(const wchar_t* data)
{
    // First insert string length
    auto length = static_cast<std::uint32_t>(std::wcslen(data));
    *this << length;

    // Then insert characters
    for (const wchar_t* c = data; *c != L'\0'; ++c)
        *this << static_cast<std::uint32_t>(*c);

    return *this;
}

Action& Action::operator<<(const std::wstring& data)
{
    // First insert string length
    auto length = static_cast<std::uint32_t>(data.size());
    *this << length;

    // Then insert characters
    if (length > 0)
    {
        for (wchar_t c : data)
            *this << static_cast<std::uint32_t>(c);
    }

    return *this;
}

bool Action::checkSize(std::size_t size)
{
    m_isValid = m_isValid && (m_readPos + size <= m_data.size());

    return m_isValid;
}