#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>

#include <map>

class Router: public Poco::Net::HTTPRequestHandlerFactory
{
    public:
        void addRoute(const std::string& url, const std::string& resource);
        Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

    private:
        void init();
        Poco::Net::HTTPRequestHandler* getResouce(const std::string&);

    private:
        std::map<std::string, std::string> m_routingTable;

};
