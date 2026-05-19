#include <iostream>
#include <exception>
#include "http.h"
#include "http_status.h"
#include "http_json.h"

int main()
{
    try
    {
        http::Session session("http://httpbin.org");
        session.set_header("User-Agent", "libhttp/2.0");
        http::RequestOptions options;
        options.allow_redirects = true;
        options.retry_count = 1;
        options.retry_delay_ms = 100;

        // Example 0: Authorization Methods
        std::cout << "=== Authorization Methods ===" << std::endl;

        // Method 1: Bearer Token (JWT, OAuth)
        std::cout << "\n1. Bearer Token Authorization:" << std::endl;
        try
        {
            session.set_header("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...");
            http::Response auth_response = session.get("/get", options);
            std::cout << "Status: " << auth_response.status_code << std::endl;
            std::cout << "Bearer token sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "Bearer token request failed: " << e.what() << std::endl;
        }

        // Method 2: Basic Authentication (username:password)
        std::cout << "\n2. Basic Authentication:" << std::endl;
        try
        {
            // Format: Base64 encode "username:password"
            // Example: "user:password" -> "dXNlcjpwYXNzd29yZA=="
            session.set_header("Authorization", "Basic dXNlcjpwYXNzd29yZA==");
            http::Response basic_auth_response = session.get("/get", options);
            std::cout << "Status: " << basic_auth_response.status_code << std::endl;
            std::cout << "Basic authentication sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "Basic auth request failed: " << e.what() << std::endl;
        }

        // Method 3: API Key in Header
        std::cout << "\n3. API Key in Header:" << std::endl;
        try
        {
            session.set_header("X-API-Key", "your-api-key-here-1234567890");
            http::Response api_key_response = session.get("/get", options);
            std::cout << "Status: " << api_key_response.status_code << std::endl;
            std::cout << "API key sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "API key request failed: " << e.what() << std::endl;
        }

        // Method 4: API Key in Query Parameters
        std::cout << "\n4. API Key in Query Parameters:" << std::endl;
        try
        {
            http::RequestOptions api_key_params;
            api_key_params.params["api_key"] = "your-api-key-here-1234567890";
            api_key_params.params["user_id"] = "12345";
            
            http::Response api_key_query_response = session.get("/get", api_key_params);
            std::cout << "Status: " << api_key_query_response.status_code << std::endl;
            std::cout << "API key in query params sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "API key query params request failed: " << e.what() << std::endl;
        }

        // Method 5: Custom Authorization Header
        std::cout << "\n5. Custom Authorization Header:" << std::endl;
        try
        {
            session.set_header("Authorization", "Digest username=user, realm=example.com, nonce=abc123");
            http::Response custom_auth_response = session.get("/get", options);
            std::cout << "Status: " << custom_auth_response.status_code << std::endl;
            std::cout << "Custom authorization sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "Custom auth request failed: " << e.what() << std::endl;
        }

        // Method 6: OAuth Token with Refresh
        std::cout << "\n6. OAuth Token (Access Token):" << std::endl;
        try
        {
            session.set_header("Authorization", "Bearer access_token_xyz789");
            session.set_header("X-Refresh-Token", "refresh_token_abc123");
            http::Response oauth_response = session.get("/get", options);
            std::cout << "Status: " << oauth_response.status_code << std::endl;
            std::cout << "OAuth token sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "OAuth request failed: " << e.what() << std::endl;
        }

        std::cout << std::endl;

        // Example 1: GET Request
        std::cout << "=== GET Request ===" << std::endl;
        try
        {
            http::Response get_response = session.get("/get", options);
            std::cout << "Status: " << get_response.status_code << " - " << get_response.reason << std::endl;
            std::cout << "Is Success: " << (get_response.is_success() ? "Yes" : "No") << std::endl;
            std::cout << "Content-Type: " << get_response.content_type() << std::endl;
            std::cout << "Response body (first 300 chars): " << std::endl;
            std::cout << get_response.body.substr(0, 300) << "..." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "GET request failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 2: GET with Query Parameters
        std::cout << "=== GET with Query Parameters ===" << std::endl;
        try
        {
            http::RequestOptions get_options;
            get_options.params["page"] = "1";
            get_options.params["limit"] = "10";
            get_options.params["search"] = "test";
            
            http::Response param_response = session.get("/get", get_options);
            std::cout << "Status: " << param_response.status_code << std::endl;
            std::cout << "Query params sent successfully" << std::endl;
            if (param_response.is_json())
            {
                std::cout << "Response is JSON format" << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "GET with params failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 3: POST Request with JSON Body
        std::cout << "=== POST Request with JSON Body ===" << std::endl;
        try
        {
            // Create JSON payload
            http::json::Value payload;
            payload["name"] = "John Doe";
            payload["email"] = "john@example.com";
            payload["age"] = 30;
            payload["active"] = true;

            http::RequestOptions post_options;
            post_options.json = payload;
            post_options.allow_redirects = true;

            http::Response post_response = session.post("/post", post_options);
            std::cout << "Status: " << post_response.status_code << " - " << post_response.reason << std::endl;
            std::cout << "Is Success: " << (post_response.is_success() ? "Yes" : "No") << std::endl;
            std::cout << "Response body (first 300 chars): " << std::endl;
            std::cout << post_response.body.substr(0, 300) << "..." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "POST request failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 4: POST with Form Data
        std::cout << "=== POST with Form Data ===" << std::endl;
        try
        {
            http::RequestOptions form_options;
            form_options.data["username"] = "johndoe";
            form_options.data["password"] = "secret123";
            form_options.data["remember"] = "true";

            http::Response form_response = session.post("/post", form_options);
            std::cout << "Status: " << form_response.status_code << std::endl;
            std::cout << "Form data sent successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "POST with form data failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 5: POST with Custom Headers & Authorization
        std::cout << "=== POST with Custom Headers & Authorization ===" << std::endl;
        try
        {
            // Set authorization headers
            session.set_header("Authorization", "Bearer your-token-here");
            session.set_header("X-Custom-Header", "CustomValue");
            session.set_header("X-Request-ID", "12345-67890");

            http::json::Value data;
            data["message"] = "Test message";
            data["timestamp"] = "2026-05-17T12:00:00Z";

            http::RequestOptions custom_options;
            custom_options.json = data;

            http::Response custom_response = session.post("/post", custom_options);
            std::cout << "Status: " << custom_response.status_code << std::endl;
            std::cout << "Custom headers and authorization sent with POST request" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "POST with custom headers failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 6: PUT Request
        std::cout << "=== PUT Request ===" << std::endl;
        try
        {
            http::json::Value update_payload;
            update_payload["id"] = 123;
            update_payload["name"] = "Jane Doe";
            update_payload["email"] = "jane@example.com";
            update_payload["status"] = "updated";

            http::RequestOptions put_options;
            put_options.json = update_payload;

            http::Response put_response = session.put("/put", put_options);
            std::cout << "Status: " << put_response.status_code << " - " << put_response.reason << std::endl;
            std::cout << "Is Success: " << (put_response.is_success() ? "Yes" : "No") << std::endl;
            std::cout << "Resource updated successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "PUT request failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 7: PUT with Partial Update
        std::cout << "=== PUT with Partial Update ===" << std::endl;
        try
        {
            http::json::Value partial_update;
            partial_update["status"] = "active";
            partial_update["last_modified"] = "2026-05-17";

            http::RequestOptions patch_options;
            patch_options.json = partial_update;

            http::Response patch_response = session.put("/put", patch_options);
            std::cout << "Status: " << patch_response.status_code << std::endl;
            std::cout << "Partial update sent" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "PUT partial update failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 8: DELETE Request
        std::cout << "=== DELETE Request ===" << std::endl;
        try
        {
            http::RequestOptions delete_options;
            
            http::Response delete_response = session.delete_("/delete", delete_options);
            std::cout << "Status: " << delete_response.status_code << " - " << delete_response.reason << std::endl;
            std::cout << "Is Success: " << (delete_response.is_success() ? "Yes" : "No") << std::endl;
            std::cout << "Resource deleted successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "DELETE request failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 9: DELETE with Query Parameters
        std::cout << "=== DELETE with Query Parameters ===" << std::endl;
        try
        {
            http::RequestOptions delete_with_params;
            delete_with_params.params["id"] = "123";
            delete_with_params.params["force"] = "true";

            http::Response delete_param_response = session.delete_("/delete", delete_with_params);
            std::cout << "Status: " << delete_param_response.status_code << std::endl;
            std::cout << "DELETE with parameters sent" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "DELETE with params failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 10: Complete CRUD Workflow
        std::cout << "=== Complete CRUD Workflow ===" << std::endl;
        try
        {
            // CREATE
            std::cout << "Step 1: Creating resource..." << std::endl;
            http::json::Value create_data;
            create_data["title"] = "New Post";
            create_data["content"] = "This is a test post";
            create_data["author"] = "John Doe";

            http::RequestOptions create_opts;
            create_opts.json = create_data;
            http::Response create_resp = session.post("/post", create_opts);
            std::cout << "  - Create Status: " << create_resp.status_code << std::endl;

            // READ
            std::cout << "Step 2: Reading resource..." << std::endl;
            http::Response read_resp = session.get("/get", options);
            std::cout << "  - Read Status: " << read_resp.status_code << std::endl;

            // UPDATE
            std::cout << "Step 3: Updating resource..." << std::endl;
            http::json::Value update_data;
            update_data["title"] = "Updated Post";
            update_data["content"] = "This post has been updated";
            update_data["author"] = "Jane Doe";

            http::RequestOptions update_opts;
            update_opts.json = update_data;
            http::Response update_resp = session.put("/put", update_opts);
            std::cout << "  - Update Status: " << update_resp.status_code << std::endl;

            // DELETE
            std::cout << "Step 4: Deleting resource..." << std::endl;
            http::RequestOptions delete_opts;
            http::Response delete_resp = session.delete_("/delete", delete_opts);
            std::cout << "  - Delete Status: " << delete_resp.status_code << std::endl;

            std::cout << "CRUD workflow completed successfully!" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "CRUD workflow failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Example 11: Error Handling for CRUD
        std::cout << "=== Error Handling ===" << std::endl;
        try
        {
            http::Response error_resp = session.get("/status/404", options);
            if (error_resp.is_error())
            {
                std::cout << "Error Status: " << error_resp.status_code << " - " << error_resp.reason << std::endl;
                std::cout << "Is 404: " << (error_resp.status_code == 404 ? "Yes" : "No") << std::endl;
                std::cout << "Is Server Error: " << (error_resp.is_server_error() ? "Yes" : "No") << std::endl;
            }
            
            // Uncomment to raise exception on error status
            // error_resp.raise_for_status();
        }
        catch (const std::exception &e)
        {
            std::cout << "Error handling: " << e.what() << std::endl;
        }
    }
    catch (const std::exception &error)
    {
        std::cout << "Fatal error: " << error.what() << '\n';
    }

    return 0;
}
