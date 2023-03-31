#include "MeetingLib/Database.hpp"
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border


#include <iostream>
#include <ftxui/dom/table.hpp>

ftxui::Element render_meeting_table(const std::vector<MeetingLib::Meeting>& meetings) {
    using namespace ftxui;
    auto table = std::vector<Element>{};
    table.reserve(meetings.size() + 2);
    table.push_back({ separator() });
    auto top_row = hbox(
        text("Id"),
        separator(),
        text("Date[yyyy/mm/dd]"),
        separator(),
        text("Time[h]"),
        separator(),
        text("Duration[h]")
    );
    table.push_back(top_row);

    for(const auto meeting: meetings) {
        auto row = hbox(
                text(std::to_string(*meeting.get_id())),
                separator(),
                text(MeetingLib::date_to_string(meeting.get_date())),
                separator(),
                text(std::to_string(meeting.get_start().count())),
                separator(),
                text(std::to_string(meeting.get_duration().count()))
                );

        table.push_back(row);
    }
    return vbox(table);
}

int main() {
    using namespace ftxui;
    using namespace MeetingLib;

    Database db{ "meeting.sqlite" };

    ScreenInteractive screen{ ScreenInteractive::Fullscreen() };

    // -----------------------------------------------
    std::vector<Meeting> meetings_view{};
    std::string username_view{};
    std::string error_view{};

    auto input_username_view_options = InputOption{};

    input_username_view_options.on_enter = [&db, &username_view, &error_view, &screen]() {
        if(!is_nickname_valid(username_view)) {
            error_view = "Invalid username!";
            return;
        }

        auto [user, find_code] = db.find_user(username_view);

        if(find_code == Database::ErrorCode::userNotFound) {
            error_view = "User does not exist!";
            return;
        } else if(find_code != Database::ErrorCode::ok) {
            error_view = "Error finding user!";
            return;
        }

        error_view = "";
        auto [meetings, get_code] = db.get_user_meetings(*user->get_id());
    };
    auto input_username_view = Input(&username_view, "User's nickname", input_username_view_options);

    // -------------------------------------------------
    std::string username_create{};
    std::string error_create{};

    auto create_username_button =
            Button( "Submit", [&db, &username_create, &error_create, &screen]() {
        if(!is_nickname_valid(username_create)) {
            error_create = "Invalid username!";
            return;
        }

        auto user = User{ username_create };
        auto [id, add_code] = db.add_user(user);

        if(add_code == Database::ErrorCode::userAlreadyExists) {
            error_create = "User already exists!";
            return;
        } else if(add_code != Database::ErrorCode::ok) {
            error_create = "Error creating user!";
            return;
        }

        screen.Exit();
    });
    auto input_username_create = Input(&username_create, "Enter user's nickname: ");

    enum class Option: int {
        create = 0,
        arrange = 1,
        view = 2,
        exit = 3,
    };

    static std::vector<std::string> entries = {
            "Create user",
            "Arrange meeting",
            "View meetings",
            "Exit"
    };
    int menu_selected{ 0 };
    MenuOption option = MenuOption::HorizontalAnimated();
    option.on_enter = [&]() {
        switch(static_cast<Option>(menu_selected)) {
            case Option::create: {
                auto exit_button = Button("Exit", [&] {
                    username_create = "";
                    error_create = "";
                    screen.Exit();
                });

                auto component = Container::Vertical({
                                                             input_username_create,
                                                             create_username_button,
                                                             exit_button
                                                     });

                auto renderer = Renderer(component, [&] {
                    return vbox(
                            {
                                    text("Create a new user"),
                                    hbox(text(" Username : "), input_username_create->Render()),
                                    create_username_button->Render(),
                                    exit_button->Render(),
                                    hbox(text( error_create)),
                            }
                    );
                });
                screen.Loop(renderer);
            }
                break;
            case Option::arrange:
                // TODO
                break;
            case Option::view: {
                auto exit_button = Button("Exit", [&] {
                    username_view = "";
                    error_view = "";
                    screen.Exit();
                });

                auto component = Container::Vertical({
                                                         input_username_view,
                                                         exit_button,
                                                     });

                auto renderer = Renderer(component, [&] {
                    return vbox(
                            {
                                text("View user's meetings"),
                                hbox(text(" Username : "), input_username_view->Render()),
                                text( error_view),
                                exit_button->Render(),
                                render_meeting_table(meetings_view)
                            }
                    );
                });
                screen.Loop(renderer);
            }
                break;
            case Option::exit:
                screen.Exit();
                break;
        }
    };
    auto menu = Component(Menu(&entries, &menu_selected, &option));

    screen.Loop(menu);
}