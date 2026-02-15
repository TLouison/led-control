#ifndef SERVER_H
#define SERVER_H

#include <Gpio.h>

#include <httplib.h>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Server
{
    class GpioServer
    {
    public:
        GpioServer(Gpio::Manager &manager)
        {
            // Set up routes
            svr.Get("/led/status", [this, &manager](const httplib::Request &, httplib::Response &res)
                    { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        res.set_content(this->get_level_response(led).dump(), "application/json"); });
            svr.Post("/led/on", [this, &manager](const httplib::Request &, httplib::Response &res)
                     { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        led->set_high();
                        res.set_content(this->get_generic_success().dump(), "application/json"); });
            svr.Post("/led/off", [this, &manager](const httplib::Request &, httplib::Response &res)
                     { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        led->set_low();
                        res.set_content(this->get_generic_success().dump(), "application/json"); });
            svr.Post("/stop", [this, &manager](const httplib::Request &, httplib::Response &res)
                     { this->svr.stop(); });
        }

        ~GpioServer()
        {
            std::cout << "Server has shut down." << std::endl;
        }

        std::thread run_server()
        {
            // Run the server in a separate thread
            return std::thread([&]()
                               {
                std::cout << "Server listening on http://localhost:8080" << std::endl;
                // This call is blocking
                if (!this->svr.listen("0.0.0.0", 8080))
                {
                    std::cerr << "Server stopped in error state" << std::endl;
                } });
        }

        json get_generic_success()
        {
            return {{"success", true}};
        }

        json get_level_response(Gpio::Pin *led)
        {
            return {{"level", led->get_level()}, {"success", true}};
        }

    private:
        httplib::Server svr;
        std::thread svr_t;
    };
};

#endif
