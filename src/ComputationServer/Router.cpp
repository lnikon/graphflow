#pragma once

#include "Router.h"

void Router::addRoute(const std::string& url, const std::string& resource)
{
    poco_assert(!url.empty());
    poco_assert(!resource.empty());

    m_routingTable[url] = resource;
}

Poco::Net::HTTPRequestHandler* Router::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return nullptr;
}

void Router::init()
{
    addRoute("/healthz");
}

Poco::Net::HTTPRequestHandler* Router::getResouce(const std::string& route)
{
    Poco::Net::URI uri = Poco::Net::URI(route);
    auto factoryIndex = routingTable.find(uri.getPath());
    if (factoryIndex == routingTable.end())
    {
        // TODO: Return concrete implementation
        return nullptr;
    }
    
    // TODO: Return concrete implementation
    return nullptr;
}

