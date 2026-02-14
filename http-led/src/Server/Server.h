#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Server
{
    class GpioServer
    {
    public:
        GpioServer(Gpio::GpioManager manager)
        {
            // Set up routes
            svr.Get("/led/status", [&manager](const httplib::Request &, httplib::Response &res)
                    { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        res.set_content(get_level_response(led.get()).dump(), "application/json"); });
            svr.Post("/led/on", [&manager](const httplib::Request &, httplib::Response &res)
                     { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        led->set_high();
                        res.set_content(get_generic_success().dump(), "application/json"); });
            svr.Post("/led/off", [&manager](const httplib::Request &, httplib::Response &res)
                     { 
                        Gpio::Pin* led = manager.getPinWithName("led");
                        led->set_low();
                        res.set_content(get_generic_success().dump(), "application/json"); });
            svr.Post("/stop", [&manager](const httplib::Request &, httplib::Response &res)
                     { svr.stop(); });

            // Run the server in a separate thread
            std::thread svr_t([&]()
                              {
                std::cout << "Server listening on http://localhost:8080" << std::endl;
                // This call is blocking
                if (!this.svr.listen("0.0.0.0", 8080))
                {
                    std::cerr << "Server stopped in error state" << std::endl;
                } });
        }

        ~GpioServer()
        {
            svr_t.join();
            std::cout << "Server has shut down." << std::endl;
        }

        json get_generic_success()
        {
            return {{"success", true}};
        }

        json get_level_response(Pin *led)
        {
            return {{"level", led->get_level()}, {"success", true}};
        }

    private:
        httplib::Server svr;
        std::thread svr_t;
    };
};

#endif
