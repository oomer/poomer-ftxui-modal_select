// This program is a simple example of how to use the ftxui library to create a modal dialog.
// It allows the user to select an address from a list of addresses in a modal dialog.
// It allows the user to select a file from a list of files in a modal dialog.
// It allows the user to enter an address manually.
// It creates a JSON file to store the addresses.

// FTXUI is a library that allows you to create text-based user interfaces.
// It is a lightweight library that is easy to use and understand.
// It is a modern C++ library that provides a functional approach to building terminal applications.

#include <memory>  // for allocator, shared_ptr, __shared_ptr_access, used in ftxui
#include <string>  // for string, basic_string, char_traits, operator+
#include <vector>  // for vector, for server_address_labels and bsz_files
#include <regex>   // for regex validation , for ipv4 and fqdn validation
#include <iostream>// for cout, cerr
#include <fstream> // for ifstream, ofstream, for reading and writing to servers.json
#include <json.hpp>// for nlohmann/json, for parsing and writing to servers.json
#include <thread>  // for std::this_thread, for running the progress animation in a separate thread
#include <chrono>  // for std::chrono, for sleeping the progress animation

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Renderer, Horizontal, Tab
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, Element, filler, text, hbox, separator, center, vbox, bold, border, clear_under, dbox, size, GREATER_THAN, HEIGHT

int main() {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  // There are two layers. One at depth = 0 and the modal window at depth = 1;
  int depth = 0;
  float percentage = 0.0f; 
  
  // Define different modal types using enum for clarity
  enum ModalType {
    MAIN_SCREEN = 0,
    SERVER_SELECTION = 1,
    FILE_SELECTION = 2
  };

  // The current rating of FTXUI.
  std::string address = "";
  std::string address_input = "";
  std::string console_message = "Welcome to Bellatui 0.5\n Running in client mode";
  std::string validation_message = "";
  
  std::vector<std::string> server_address_labels = {
      "localhost", 
  };
  
  // Sample BSZ files for the file selection modal
  std::vector<std::string> bsz_files = {
      "scene1.bsz",
      "scene2.bsz",
      "example.bsz",
      "test_scene.bsz"
  };
  std::string selected_bsz_file = "No file selected";
  int selected_file_index = 0;

  bool address_valid = false;

  // Function to validate IPv4 address
  // Uses a lambda function by inline definition
  // The benefit of this is that it is a local function to the main function
  // and it has access to the variables in the main function
  // For larger functions, it is better to define them outside the main function
  auto validate_ipv4 = [](const std::string& ip) -> bool {
    std::regex ipv4_pattern(
        "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, ipv4_pattern);
  };

  // Function to validate FQDN
  // Uses a lambda function by inline definition
  auto validate_fqdn = [](const std::string& fqdn) -> bool {
    // Basic FQDN validation: letters, numbers, hyphens, dots
    // At least one dot, no consecutive dots, no leading/trailing dots
    // Each label max 63 chars, total max 253 chars
    if (fqdn.length() > 253) return false;
    if (fqdn.find('.') == std::string::npos) return false;
    std::regex fqdn_pattern(
        "^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,}$");
    return std::regex_match(fqdn, fqdn_pattern);
  };

  // Combined validation function
  // Uses a lambda function by inline definition
  // We need to combine because we need to validate both IPv4 and FQDN
  auto validate_address = [&]() {
    if (validate_ipv4(address_input)) {
      address_valid = true;
      return;
    }
    
    if (validate_fqdn(address_input)) {
      address_valid = true;
      return;
    }
    console_message = "Invalid address format";
    address_valid = false;
  };

  // At depth=0, two buttons. One for rating FTXUI and one for quitting.
  auto ftxui_button_bsz = Button("Select .bsz", [&] { depth = FILE_SELECTION; });
  auto ftxui_button_address = Button("Select Server", [&] { depth = SERVER_SELECTION; });
  auto ftxui_input_address = Input(&address_input, "FQDN or IP address");

  auto ftxui_button_render = Button("Upload and render", [&] { 
    depth = MAIN_SCREEN;
    
    // Reset percentage to start animation
    percentage = 0.0f;
    
    // Launch a separate thread for updating the progress
    // This is a lambda function that will be called when the button is clicked
    // This is a stand-in for the actual uploading process
    // [TODO] tie this into the ZMQ chunk upload process
    // [TODO] chedck the server address is valid
    // [TODO] check the server is reachable
    // [TODO] check the file is a valid .bsz file
    
    std::thread animation_thread([&]() {
      // Update percentage from 0 to 100% in small increments
      for (float p = 0.0f; p <= 1.0f; p += 0.01f) {
        percentage = p;
        
        // Post an event to trigger a UI redraw
        screen.PostEvent(Event::Custom);
        
        // Sleep to control animation speed
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
      }
      
      // Ensure we reach exactly 100%
      percentage = 1.0f;
      screen.PostEvent(Event::Custom);
    });
    
    // Detach the thread so it continues running independently
    animation_thread.detach();
  });

  auto ftxui_button_quit = Button("Quit", screen.ExitLoopClosure());

  // 1. Read existing JSON file
  nlohmann::json j;
  try {
      std::ifstream input_file("servers.json");
      if (!input_file.is_open()) 
      {
          // Create an empty servers.json file with basic structure
          console_message = "servers.json not found. Creating new empty file.";
          // Initialize with empty bellatui_servers array
          j["bellatui_servers"] = nlohmann::json::array();
          // Save the initial empty structure
          std::ofstream new_file("servers.json");
          if (!new_file.is_open()) 
          {
              std::cerr << "Failed to create new servers.json file!" << std::endl;
              return 1;
          }
          new_file << j.dump(2);
          new_file.close();
          
          std::cout << "Created new data.json with empty servers list" << std::endl;
      } else 
      {
          input_file >> j;  // Parse JSON directly into j
          input_file.close();

          // Read the server addresses from the JSON file
          for (const auto& server : j["bellatui_servers"]) {
              server_address_labels.push_back(server["address"]);
          }
      }
  } catch (const nlohmann::json::parse_error& e) 
  {
      std::cout << "JSON parse error: " + std::string(e.what()) << std::endl;
      return 1;
  }

  // FTXUI is a library that allows you to create text-based user interfaces.
  // It is a modern C++ library that provides a functional approach to building terminal applications.
  // Here are some notes on FTXUI:
  // Elements are visual, non-interactive parts of the UI:
  // Components are interactive parts of the UI.
  // A Container is a component that contains other components.
  // A component is a function that returns an Element
  // Renderer is a component that renders a component
  // CatchEvent is a component that catches events from the user

  // Create a component that renders text
  // Containers cannot contain elements, so we need to create a component that renders text
  auto ftxui_text_component = Renderer
  ([]{
      return text("Add server address");
  });
  
  auto depth_0_right_container = Container::Vertical
  ({
      ftxui_button_render,
      ftxui_button_quit,
  });

  auto depth_0_address_container = Container::Vertical
  ({
      ftxui_text_component,
      ftxui_input_address | vscroll_indicator 
                          | size(HEIGHT, EQUAL, 2) 
                          | size(WIDTH, EQUAL, 30),
  }) | CatchEvent([&](Event event) {
      // Check if the Return/Enter key was pressed
      if (event == Event::Return) { // Handle the Return key press
          validate_address();
          if (address_valid) 
          {
            console_message = "Server address " + address_input + " added";
          } else {
            address_input = ""; // Clear the input if the address is invalid
          }
          
          // You can perform actions here, like:
          // - Validating the input
          // - Submitting the form
          // - Changing focus
          // - Updating state

          // Add the new server to the JSON 
          j["bellatui_servers"].push_back(nlohmann::json{
              {"address", address_input},
              {"commandport", 22},
              {"heartport", 22},
              {"pubkeyport", 22}
          });
          // Add the new server to the server_address_labels vector
          server_address_labels.push_back(address_input);
          address_input=""; // Clear the input field

          std::ofstream output_file("servers.json");
          if (!output_file.is_open()) 
          {
              std::cerr << "Could not open servers.json for writing!" << std::endl;
              return true;
          }
          output_file << j.dump(2);
          output_file.close(); 

          // FTXUI is a functional library, so it is important to return true to indicate that the event has been handled
          // If you return false, the event will be passed to other components
          return true;
      }
      // Return false for other events to let them propagate to child components
      return false;
  });
  
  // Container::Horizontal contain nest other containers and components
  auto depth_0_container = Container::Horizontal
  ({
      ftxui_button_bsz,
      ftxui_button_address,
      depth_0_address_container,
      depth_0_right_container,
  });

  // Renderer is a component that renders a component
  auto depth_0_renderer = Renderer(depth_0_container, [&] {
    return vbox({
               text("Demo modal / menu / json / files"),
               separator(),
               hbox({
                 text("Selected file: ") | bold,
                 text(selected_bsz_file),
               }),
               hbox({
                 text("Selected server: ") | bold,
                 text(address.empty() ? "None" : address),
               }),
               // Display percentage value alongside the gauge
               hbox({
                 text(std::to_string(int(percentage * 100)) + "% "),
                 gauge(percentage) | flex,
               }),
               filler(),
               separator(),
               // Render the container's components
               depth_0_container->Render(),  
               paragraph(console_message) | bold 
                                          | size(WIDTH, EQUAL, 30)
                                          | size(HEIGHT, EQUAL, 3),
           }) |
           border | size(HEIGHT, GREATER_THAN, 18) | center;
  });

  // This is a lambda function that will be called when a menu item is selected
  auto on_address = [&](std::string new_address) {
    address = new_address;
    depth = MAIN_SCREEN; // switch back to the main menu
  };
  
  // Create a menu component for server selection
  int selected_server_index = 0;
  auto server_menu = Menu(&server_address_labels, &selected_server_index)
    | CatchEvent([&](Event event) {
        if (event == Event::Return) { // When Enter pressed, select that server
          on_address(server_address_labels[selected_server_index]);
          console_message = "";
          depth = MAIN_SCREEN;
          return true;
        }
        if (event == Event::Character("-")) { // When minus key pressed, remove the selected server
          // Make sure we have at least one server and a valid selection
          if (!server_address_labels.empty() && selected_server_index >= 0 && 
              selected_server_index < static_cast<int>(server_address_labels.size())) {
            
            // Get the address to remove
            std::string address_to_remove = server_address_labels[selected_server_index];
            
            // Remove from the vector so display is updated
            server_address_labels.erase(server_address_labels.begin() + selected_server_index);
            
            // Update the selected index to avoid out-of-bounds
            if (selected_server_index >= static_cast<int>(server_address_labels.size())) {
              selected_server_index = std::max(0, static_cast<int>(server_address_labels.size()) - 1);
            }
            
            // Remove from the JSON so it is not saved
            auto& servers = j["bellatui_servers"];
            for (size_t i = 0; i < servers.size(); ++i) {
              if (servers[i]["address"] == address_to_remove) {
                servers.erase(servers.begin() + i);
                break;
              }
            }
            
            // Save the updated JSON
            std::ofstream output_file("servers.json");
            if (output_file.is_open()) {
              output_file << j.dump(2); // 2 is the indent level
              output_file.close();
              console_message = "Server address '" + address_to_remove + "' removed";
            } else {
              console_message = "Error: Could not save changes to servers.json";
            }
          }
          return true;
        }
        if (event == Event::Escape) {
          depth = MAIN_SCREEN;
          return true;
        }

        return false;
      })
    | size(HEIGHT, LESS_THAN, 10)  // Limit height but allow scrolling
    | border
    | vscroll_indicator;  // Add a scroll indicator for when the list gets long

  auto depth_1_container = Container::Vertical({
      server_menu,
  });

  auto depth_1_renderer = Renderer(depth_1_container, [&] {
    return vbox({
               text("Select bellatui server"),
               separator(),
               filler(),
               depth_1_container->Render(),  // This now renders the menu
               filler(),
               text("Enter selects, - deletes, Esc cancels") | center,
           }) |
           border;
  });

  // This is a lambda function that will be called when a file is selected
  auto on_file_select = [&](std::string file_name) {
    selected_bsz_file = file_name;
    depth = MAIN_SCREEN; // switch back to the main menu
    console_message = "Selected file: " + file_name;
  };

  // Create a menu component for file selection
  auto file_menu = Menu(&bsz_files, &selected_file_index)
    | CatchEvent([&](Event event) {
        // When Enter is pressed on a menu item, select that file
        if (event == Event::Return) {
          on_file_select(bsz_files[selected_file_index]);
          return true;
        }
        // When Escape is pressed, return to main screen
        if (event == Event::Escape) {
          depth = MAIN_SCREEN;
          return true;
        }
        return false;
      })
    | size(HEIGHT, LESS_THAN, 10)  // Limit height but allow scrolling
    | border
    | vscroll_indicator;  // Add a scroll indicator for when the list gets long

  // Create a container for the file selection modal
  auto depth_2_container = Container::Vertical({
      file_menu,
  });

  // Create a renderer for modal file dialog
  auto depth_2_renderer = Renderer(depth_2_container, [&] {
    return vbox({
               text("Select .bsz File") | bold,
               separator(),
               filler(),
               depth_2_container->Render(),
               filler(),
               text("Press Enter to select, Escape to cancel") | center,
           }) |
           border;
  });

  // Create a container for the main screen
  auto main_container = Container::Tab( //Tab allows switching between elements
      {
          depth_0_renderer,
          depth_1_renderer,
          depth_2_renderer,
      },
      &depth);

  // Main renderer is the main component that renders the main container
  auto main_renderer = Renderer(main_container, [&] {
    Element document = depth_0_renderer->Render();

    // Show the appropriate modal based on depth value
    if (depth == SERVER_SELECTION) {
      document = dbox({ //dbox allows stacking of elements
          document,
          depth_1_renderer->Render() | clear_under | center,
      });
    }
    else if (depth == FILE_SELECTION) {
      document = dbox({ //dbox allows stacking of elements
          document,
          depth_2_renderer->Render() | clear_under | center,
      });
    }
    return document;
  });

  // ftxui main loop
  // loop will run until the user quits the application
  // [TODO] other threads need to be added before this loop
  // [TODO] ie ZMQ connections, etc
  screen.Loop(main_renderer);

  return 0;
}

