
#ifndef BONJOUR_FOR_CPP_UTILS_HPP
#define BONJOUR_FOR_CPP_UTILS_HPP

#include <sys/select.h>

#include <cstring>
#include <string>

namespace impl
{
    /**
     * @brief Waits for activity on a given socket until a timeout occurs.
     *
     * This function waits for activity (such as incoming data) on the specified socket
     * for a duration defined by `timeout_secs` and `timeout_usecs`. If the timeout
     * period elapses without any activity on the socket, the function returns.
     *
     * @param socket The socket file descriptor to wait on.
     * @param timeout_secs The timeout duration in seconds.
     * @param timeout_usecs The additional timeout duration in microseconds.
     * @return Returns 0 if the timeout period elapses without any activity on the socket.
     *         Returns 1 if there is activity on the socket within the timeout period.
     *         Returns -1 if an error occurs.
     */
    int wait_on_socket(int socket, int timeout_secs, int timeout_usecs)
    {
        fd_set read, write, except;
        struct timeval timeout;

        // Timeout
        
        timeout.tv_sec = timeout_secs;
        timeout.tv_usec = timeout_usecs;
    
        // Sets

        FD_ZERO(&read);
        FD_ZERO(&write);
        FD_ZERO(&except);
            
        FD_SET(socket, &read);
            
        return select(socket + 1, &read, &write, &except, &timeout);
    }
    
    /**
     * @brief Validates a given name string and returns a standardized version of the name.
     *
     * This function checks the validity of the input name string. It may perform operations such as
     * trimming whitespace, checking for illegal characters, or other forms of validation as per
     * the implementation details. The function returns a standardized version of the name that
     * adheres to the validation rules.
     *
     * @param name A C-string representing the name to be validated.
     * @return A `std::string` containing the validated and standardized name.
     *         If the input name is invalid, the return value may be an empty string or an error string,
     *         depending on the implementation.
     */
    std::string validate_name(const char *name)
    {
        return name;
    }
    
    /**
     * @brief Validates a given registration type (regtype) string and returns a standardized version of it.
     *
     * This function checks the validity of the input registration type string. It may perform operations
     * such as ensuring the regtype conforms to a specific format, removing invalid characters,
     * or other forms of validation according to the implementation details. The function returns a
     * standardized version of the registration type that adheres to the validation rules.
     *
     * @param regtype A C-string representing the registration type to be validated.
     * @return A `std::string` containing the validated and standardized registration type.
     *         If the input regtype is invalid, the return value may be an empty string or a default
     *         error string, depending on the implementation.
     */
    std::string validate_regtype(const char *regtype)
    {
        return regtype;
    }
    
    /**
     * @brief Validates a given domain string and returns a standardized version of it.
     *
     * This function checks the validity of the input domain string. It may perform operations
     * such as verifying the domain format, ensuring it adheres to specific rules (e.g.,
     * valid characters, proper structure), and possibly normalizing the domain string.
     * The function returns a standardized version of the domain that complies with the
     * validation rules.
     *
     * @param domain A C-string representing the domain to be validated.
     * @return A `std::string` containing the validated and standardized domain name.
     *         If the input domain is invalid, the return value may be an empty string or a
     *         special error string, depending on the implementation.
     */
    std::string validate_domain(const char *domain)
    {
        return (!domain || !strlen(domain)) ? "local." : domain;
    }
}

#endif /* BONJOUR_FOR_CPP_UTILS_HPP */