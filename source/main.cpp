/*
* Copyright (C) 2017 by Godlike
* This code is licensed under the MIT license (MIT)
* (http://opensource.org/licenses/MIT)
*/

#include "Application.hpp"

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

int main(int argc, char* argv[])
{
    unicorn::Settings& settings = unicorn::Settings::Instance();

    settings.Init(argc, argv, "balius_and_xanthus.log");
    settings.SetApplicationName("Balius and Xanthus");

    unicorn::UnicornRender* unicornRender = new unicorn::UnicornRender();

    if (unicornRender->Init())
    {
        Application app(settings, unicornRender);

        app.Run();
    }

    unicornRender->Deinit();
    delete unicornRender;

    unicorn::Settings::Destroy();
}
