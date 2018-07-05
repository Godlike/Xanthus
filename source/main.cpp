/*
* Copyright (C) 2017 by Godlike
* This code is licensed under the MIT license (MIT)
* (http://opensource.org/licenses/MIT)
*/

#include "Application.hpp"

#include <mule/MuleUtilities.hpp>
#include <mule/Loggers.hpp>

#include <unicorn/Settings.hpp>
#include <unicorn/Loggers.hpp>

#include <tulpar/TulparConfigurator.hpp>
#include <tulpar/Loggers.hpp>

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/file_sinks.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    auto ansiSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    ansiSink->set_level(mule::LogLevel::warn);

    auto fileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>("xanthus.log", true);

    {
        mule::Loggers::Instance().SetDefaultSettings(
            mule::Loggers::Settings{
                std::string()
                , std::string("%+")
                , mule::LogLevel::trace
                , { ansiSink, fileSink }
            }
        );
    }

    mule::MuleUtilities::Initialize();

    {
        unicorn::Loggers::Instance().SetDefaultSettings(
            unicorn::Loggers::Settings{
                std::string()
                , std::string("%+")
                , mule::LogLevel::trace
                , { ansiSink, fileSink }
            }
        );
    }

    {
        tulpar::Loggers::Instance().SetDefaultSettings(
            tulpar::Loggers::Settings{
                std::string()
                , std::string("%+")
                , mule::LogLevel::trace
                , { ansiSink, fileSink }
            }
        );
    }

    unicorn::Settings& unicornSettings = unicorn::Settings::Instance();
    tulpar::TulparConfigurator tulparSettings = {};

    unicornSettings.Init(argc, argv);
    unicornSettings.SetApplicationName("Sandbox");

    xanthus::Application app(unicornSettings, tulparSettings);

    if (app.IsValid())
    {
        app.Run();
    }
    else
    {
        std::cerr << "Failed to run application!" << std::endl;
    }

    unicorn::Settings::Destroy();

    return EXIT_SUCCESS;
}
