
#ifndef BONJOUR_PEER_HPP
#define BONJOUR_PEER_HPP

#include "bonjour_browse.hpp"
#include "bonjour_register.hpp"
#include "bonjour_service.hpp"

#include <list>

// Options for bonjour_peer

struct bonjour_peer_options
{
    enum class modes { browse_only, register_only, both };

    modes m_mode = modes::both;
    bool m_self_discover = false;
};

// An object that is a peer service (and so offers both registration and browsing)
// This object resolves peers and assumes you will poll externally when required
// It has no notification facilities

class bonjour_peer
{
public:
    /**
     * @brief Constructs a Bonjour peer with specified parameters.
     *
     * This constructor initializes a `bonjour_peer` object with the given name,
     * registration type (regtype), domain, and port. It also allows optional
     * configuration through the `bonjour_peer_options` parameter.
     *
     * @param name A constant character pointer representing the name of the Bonjour peer.
     * @param regtype A constant character pointer representing the registration type (regtype) of the service.
     * @param domain A constant character pointer representing the domain in which the service is registered.
     * @param port A 16-bit unsigned integer representing the network port on which the service is running.
     * @param options (Optional) An instance of `bonjour_peer_options` to specify additional options for the peer.
     *                If not provided, the default `bonjour_peer_options` will be used.
     *
     * @note This constructor initializes both the service registration and browsing components of the Bonjour peer.
     */
    bonjour_peer(const char *name,
                 const char *regtype,
                 const char *domain,
                 uint16_t port,
                 bonjour_peer_options options = bonjour_peer_options())
    : m_options(options)
    , m_register(name, regtype, domain, port)
    , m_browse(regtype, domain)
    {}
    
    /**
     * @brief Starts the Bonjour service for peer-to-peer communication.
     *
     * This method initializes and starts the Bonjour service for the peer-to-peer network.
     * It registers the service with the appropriate service type, port, and other
     * metadata needed for discovery by other peers in the network. If the service
     * is already running, this method will not reinitialize it.
     *
     * @return true if the service was successfully started or was already running.
     * @return false if there was an error starting the service.
     */
    bool start()
    {
        switch (m_options.m_mode)
        {
            case bonjour_peer_options::modes::browse_only:
                return m_browse.start();
                
            case bonjour_peer_options::modes::register_only:
                return m_register.start();
                
            default:
                return m_register.start() && m_browse.start();
        }
    }
    
    /**
     * @brief Stops the Bonjour service for peer-to-peer communication.
     *
     * This method stops the currently running Bonjour service and unregisters
     * it from the network, ensuring that the service is no longer discoverable
     * by other peers. If the service is not running, this method has no effect.
     *
     * @note This method does not return a value and assumes that any required
     * cleanup is handled internally.
     */
    void stop()
    {
        m_register.stop();
        m_browse.stop();
    }
    
    /**
     * @brief Clears the internal state and data of the Bonjour peer.
     *
     * This method resets and clears any internal data structures, cached information,
     * or state associated with the Bonjour peer. It effectively resets the object
     * to a clean state, but does not stop the Bonjour service if it is currently running.
     *
     * @note This method should be called when you need to reinitialize or reset the peer
     * without affecting the running service.
     */
    void clear()
    {
        m_browse.clear();
    }
    
    /**
     * @brief Retrieves the name of the Bonjour peer.
     *
     * This method returns the current name of the Bonjour peer, which is used
     * for identifying the service in the network. The name is typically a human-readable
     * string that uniquely identifies this peer among others in the same network.
     *
     * @return A constant character pointer representing the name of the Bonjour peer.
     */
    const char *name() const
    {
        return m_register.name();
    }
    
    /**
     * @brief Retrieves the registration type (regtype) of the Bonjour service.
     *
     * This method returns the registration type (regtype) of the Bonjour service, which
     * specifies the protocol and application type used for service discovery. The regtype
     * is a string that typically includes the service type and protocol, such as "_http._tcp".
     *
     * @return A constant character pointer representing the registration type of the service.
     */
    const char *regtype() const
    {
        return m_register.regtype();
    }
    
    /**
     * @brief Retrieves the domain of the Bonjour service.
     *
     * This method returns the domain in which the Bonjour service is registered.
     * The domain typically represents the network domain where the service is
     * discoverable. If no specific domain is set, it may return the default domain
     * used by the Bonjour service.
     *
     * @return A constant character pointer representing the domain of the Bonjour service.
     */
    const char *domain() const
    {
        return m_register.domain();
    }
    
    /**
     * @brief Retrieves the network port on which the Bonjour service is running.
     *
     * This method returns the port number that the Bonjour service uses for communication.
     * The port is an integral part of the service's network identity, allowing clients
     * to connect to the service on the specified port.
     *
     * @return A 16-bit unsigned integer representing the port number of the Bonjour service.
     */
    uint16_t port() const
    {
        return m_register.port();
    }
    
    /**
     * @brief Resolves the Bonjour service to obtain detailed information.
     *
     * This method performs a resolution of the Bonjour service, retrieving detailed
     * information such as the host name, port, and any associated TXT records. This
     * is typically used after discovering a service to obtain the necessary details
     * to connect to it.
     *
     * @note The method does not return a value, and the results of the resolution
     * are handled internally or through a callback mechanism.
     */
    void resolve()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        for (auto it = m_peers.begin(); it != m_peers.end(); it++)
            it->resolve();
    }
    
    /**
     * @brief Resolves the specified Bonjour service to obtain detailed information.
     *
     * This method resolves the provided Bonjour service, retrieving detailed information
     * such as the host name, port, and any associated TXT records. This is typically used
     * after discovering a service to obtain the necessary details to connect to it.
     *
     * @param service A reference to a `bonjour_named` object representing the Bonjour
     * service to be resolved.
     *
     * @note The method does not return a value, and the results of the resolution
     * are handled internally or through a callback mechanism.
     */
    void resolve(const bonjour_named& service)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        auto it = service.find(m_peers);
        
        if (it != m_peers.end())
            it->resolve();
    }
    
    /**
     * @brief Lists all discovered Bonjour peers.
     *
     * This method populates the provided list with all currently discovered Bonjour
     * peers. Each peer is represented as a `bonjour_service` object, containing
     * information such as the peer's name, regtype, domain, and port.
     *
     * @param peers A reference to a `std::list` of `bonjour_service` objects that will be
     * populated with the details of the discovered peers.
     *
     * @note The list is cleared before being populated with the current set of discovered peers.
     */
    void list_peers(std::list<bonjour_service> &peers)
    {
        std::list<bonjour_named> services;

        std::unique_lock<std::mutex> lock(m_mutex);
        
        m_browse.list_services(services);
        
        // Make sure
        // 1 - only matching items are left in m_peers
        // 2 - only non-matching items are left in services
        
        for (auto it = m_peers.begin(); it != m_peers.end(); it++)
        {
            const auto jt = it->find(services);
            
            if (jt == services.end())
                it = --m_peers.erase(it);
            else
                services.erase(jt);
        }
        
        // Add any new items to the list (noting if self-discovery is allowed)
        
        for (auto it = services.begin(); it != services.end(); it++)
        {
            if (m_options.m_self_discover || !it->equal(m_register))
                m_peers.emplace_back(*it);
        }
        
        peers = m_peers;
    }
    
private:
    
    bonjour_peer_options m_options;
    
    bonjour_register m_register;
    bonjour_browse m_browse;
    
    mutable std::mutex m_mutex;
    std::list<bonjour_service> m_peers;
};

#endif /* BONJOUR_PEER_HPP */