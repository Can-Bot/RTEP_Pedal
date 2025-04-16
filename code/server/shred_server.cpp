#include <uwebsockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

struct ShredSocket
{
    // Per-connection state could be stored here
};

// Simulated audio processor stub
void play_note(const std::string &note, float velocity)
{
    std::cout << "[DSP] Playing note: " << note << " with velocity: " << velocity << std::endl;
}

int main()
{
    uWS::App().ws<ShredSocket>("/", {.open = [](auto *ws)
                                     { std::cout << "Client connected" << std::endl; },

                                     .message = [](auto *ws, std::string_view message, uWS::OpCode opCode)
                                     {
            try
            {
                json j = json::parse(message);
                std::cout << "Received JSON:\n" << j.dump(2) << std::endl;

                // Handle different actions
                if (j.contains("action") && j["action"] == "play_note")
                {
                    std::string note = j.value("note", "");
                    float velocity = j.value("velocity", 1.0f);

                    if (!note.empty())
                    {
                        play_note(note, velocity);

                        json response = {
                            {"status", "note_received"},
                            {"note", note},
                            {"velocity", velocity}};

                        ws->send(response.dump(), opCode);
                        return;
                    }
                }

                // Fallback
                json response = {
                    {"status", "unhandled"},
                    {"data", j}};

                ws->send(response.dump(), opCode);
            }
            catch (const std::exception &e)
            {
                std::cerr << "JSON error: " << e.what() << std::endl;
                json error = {{"error", e.what()}};
                ws->send(error.dump(), opCode);
            } },

                                     .close = [](auto *ws, int code, std::string_view message)
                                     { std::cout << "Client disconnected" << std::endl; }})
        .listen(9000, [](auto *token)
                {
                    if (token)
                    {
                        std::cout << "shred WebSocket server listening on port 9000" << std::endl;
                    } })
        .run();

    return 0;
}