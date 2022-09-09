#ifndef ACTION_H
#define ACTION_H

#include <cstddef>
#include <string>
#include <vector>
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "fwd/action.h"

/**
 * @brief Utility class to build blocks of actions
 *       to be executed by the ActionHandler
 */
class Action
{
public:
    Action();

    virtual ~Action();

    //Copy
    Action(const Action&);

    //Copy assignment
    Action& operator=(const Action&);

    //Move
    Action(Action&&) noexcept;

    //Move assignment
    Action& operator=(Action&&) noexcept;

    /**
     * @brief Append data to the end of the action
     * 
     * @param data        Pointer to the sequence of bytes to append
     * @param sizeInBytes Number of bytes to append
     * 
     * @see clear
     * @see getReadPosition
     */
    void append(const void* data, std::size_t sizeInBytes);

    /**
     * @brief Clear the action
     * 
     * After calling Clear, the action is empty.
     * 
     * @see append
     */
    void clear();

public:
    // Overload of operator >> to read data from the action
    Action& operator>>(std::int8_t& data);

    Action& operator>>(std::uint8_t& data);

    Action& operator>>(std::int32_t& data);

    Action& operator>>(std::uint32_t& data);

    Action& operator>>(char* data);

    Action& operator>>(std::string& data);

    Action& operator>>(wchar_t* data);

    Action& operator>>(std::wstring& data);

    //Overload of operator << to write data into the action
    Action& operator<<(std::int8_t data);

    Action& operator<<(std::uint8_t data);
    
    Action& operator<<(std::int32_t data);
    
    Action& operator<<(std::uint32_t data);

    Action& operator<<(const char* data);

    Action& operator<<(const std::string& data);

    Action& operator<<(const wchar_t* data);

    Action& operator<<(const std::wstring& data);

private:
    /**
     * @brief Check if the packet can extract a given number of bytes
     * 
     * This function updates accordingly the state of the packet.
     * 
     * @param size Size to check
     * 
     * @return True if \a size bytes can be read from the packet
     */
    bool checkSize(std::size_t size);

    //Member data
    std::vector<char> m_data;    //!< Data stored in the action
    std::size_t       m_readPos; //!< Current reading position in the action
    std::size_t       m_sendPos; //!< Current send position in the action (for handling partial sends)
    bool              m_isValid; //!< Reading state of the action
};

#endif // ACTION_H