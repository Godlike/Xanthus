/*
* Copyright (C) 2017 by Godlike
* This code is licensed under the MIT license (MIT)
* (http://opensource.org/licenses/MIT)
*/

#include "Application.hpp"

#include <unicorn/Settings.hpp>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    unicorn::Settings& settings = unicorn::Settings::Instance();

    settings.Init(argc, argv, "sandbox.log");
    settings.SetApplicationName("Sandbox");

    xanthus::Application app(settings);

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
