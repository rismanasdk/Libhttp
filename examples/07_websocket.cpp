#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== WebSocket Example ===" << std::endl;

        std::cout << "\nWebSocket support is currently in development." << std::endl;
        std::cout << "WebSocket connections will be supported for real-time bidirectional communication." << std::endl;

        std::cout << "\nExample usage (when WebSocket is fully implemented):" << std::endl;
        std::cout << "  http::websocket::Connection ws(\"wss://echo.websocket.org\");" << std::endl;
        std::cout << "  ws.send_text(\"Hello WebSocket!\");" << std::endl;
        std::cout << "  ws.on_message([](const std::string& msg) {" << std::endl;
        std::cout << "    std::cout << \"Received: \" << msg << std::endl;" << std::endl;
        std::cout << "  });" << std::endl;

        std::cout << "\nWebSocket example completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
