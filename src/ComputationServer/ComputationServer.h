#pragma COMPUTATION_SERVER

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/SharedPtr.h>

#include <string>
#include <memory>

class ComputationServer
{
};

class RESTServer : public Poco::Util::ServerApplication
{
public:
    RESTServer(Poco::UInt16 port,
               Poco::Net::HTTPRequestHandlerFactory::Ptr router);

protected:
    int main(const std::vector<std::string>& args) override;

private:
    const Poco::UInt16 m_port;
    const std::string m_endpoint;
    const Poco::SharedPtr<Poco::Net::HTTPRequestHandlerFactory> m_router;
};
