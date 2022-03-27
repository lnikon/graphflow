#include "ComputationServer.h"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>

#include <iostream>

RESTServer::RESTServer(Poco::UInt16 port, Poco::Net::HTTPRequestHandlerFactory::Ptr router)
    : m_port(port)
    , m_endpoint("localhost")
    , m_router(router)
{
}

int RESTServer::main(const std::vector<std::string>& args)
{
    Poco::Net::HTTPServerParams::Ptr httpServerParams;
    httpServerParams->setMaxQueued(250);
    httpServerParams->setMaxThreads(50);

    Poco::Net::ServerSocket sock(m_port);
    Poco::Net::HTTPServer httpServer(m_router, std::move(sock), httpServerParams);

    std::cout << "PGASGraph REST Server started and running." << std::endl;
    std::cout << "http://" << m_endpoint << ":" << m_port << std::endl;

    auto rc{Poco::Util::Application::EXIT_OK};
    try
    {
        httpServer.start();
        waitForTerminationRequest();
        httpServer.stop();
    }
    catch (Poco::Exception& e)
    {
        rc = Poco::Util::Application::EXIT_SOFTWARE;
        std::cerr << "Error occured during execution, error=" << e.what() << std::endl;
    }
    catch (...)
    {
        rc = Poco::Util::Application::EXIT_SOFTWARE;
        std::cerr << "Unkown error occured during execution." << std::endl;
    }

    std::cout << "PGASGraph REST Server stopped." << std::endl;
    return rc;
}

