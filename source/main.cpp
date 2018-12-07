/*
* Copyright (C) 2017 by Godlike
* This code is licensed under the MIT license (MIT)
* (http://opensource.org/licenses/MIT)
*/

#include "Application.hpp"

#include <mule/MuleUtilities.hpp>
#include <mule/Loggers.hpp>

#include <unicorn/utility/Settings.hpp>
#include <unicorn/Loggers.hpp>

#include <tulpar/TulparConfigurator.hpp>
#include <tulpar/Loggers.hpp>

#include <sleipnir/SleipnirEngine.hpp>

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/file_sinks.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    auto ansiSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    ansiSink->set_level(mule::LogLevel::warn);

    auto fileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>("xanthus.log", true);

    sleipnir::SleipnirConfigurator config;

    config.globalLoggerSettings = mule::LoggerConfigBase::Settings{
        std::string()
        , std::string("%+")
        , mule::LogLevel::trace
        , { ansiSink, fileSink }
    };

    tulpar::TulparConfigurator tulparConfig;

    unicorn::utility::Settings& unicornSettings = unicorn::utility::Settings::Instance();
    unicornSettings.SetApplicationName("Sandbox");

    config.tulpar.config = &tulparConfig;
    config.unicorn.config = &unicornSettings;
    config.pegasus.config = reinterpret_cast<void*>(0x1);

    sleipnir::SleipnirEngine engine = {};
    if (engine.Initialize(config))
    {
        xanthus::Application app(engine);

        app.Run();
    }
    else
    {
        std::cerr << "Failed to initialize SleipnirEngine!" << std::endl;
    }

    unicorn::utility::Settings::Destroy();

    return EXIT_SUCCESS;
}
