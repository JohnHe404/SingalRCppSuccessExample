// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <signalrclient/hub_connection.h>
#include <signalrclient/log_writer.h>
#include <future>
class logger : public signalr::log_writer
{
	// Inherited via log_writer
	void write(const std::string& entry) override
	{
		std::cout << utility::conversions::to_utf8string(entry) << std::endl;
	}
};
void running();
int main()
{
    std::cout << "Hello World!\n";
	while (true)
	{
		std::cout << "Input yes or no:" << std::endl;
		std::string input;
		std::cin >> input;
		if (input.find("n") != -1 || input.find("N") != -1)
		{
			std::cout << "So! break" << std::endl;
			break;
		}
		else if (input.find("y") != -1 || input.find("Y") != -1)
		{
			std::cout << "Ok! running... ..." << std::endl;
			running();
		}
		else
		{
			std::cout << "So! next... ..." << std::endl;
			continue;
		}
	}
}
void ZeusSendMessage(signalr::hub_connection& connection, std::string user, std::string message)
{
	web::json::value args{};
	args[0] = web::json::value(utility::conversions::to_string_t(user));
	args[1] = web::json::value(utility::conversions::to_string_t(message));
	connection.invoke("SendMessage", args, [](const web::json::value& value, std::exception_ptr exception)
		{
			try
			{
				if (exception)
				{
					std::rethrow_exception(exception);
				}

				ucout << U("Received: ") << value.serialize() << std::endl;
			}
			catch (const std::exception& e)
			{
				ucout << U("Error while sending data: ") << e.what() << std::endl;
			}
		});
}
void ZeusSendMessageToCaller(signalr::hub_connection& connection, std::string message)
{
	web::json::value args{};
	args[0] = web::json::value(utility::conversions::to_string_t(message));
	connection.invoke("SendMessageToCaller", args, [](const web::json::value& value, std::exception_ptr exception)
		{
			try
			{
				if (exception)
				{
					std::rethrow_exception(exception);
				}

				ucout << U("Received: ") << value.serialize() << std::endl;
			}
			catch (const std::exception& e)
			{
				ucout << U("Error while sending data: ") << e.what() << std::endl;
			}
		});

}
void running()
{
	std::string user = "AI Boss";
	std::string message = "kill master";

	signalr::hub_connection connection("https://localhost:44357/ChatHub", signalr::trace_level::all, std::make_shared<logger>());
	//signalr::hub_connection connection("https://localhost:44357/chathub", signalr::trace_level::all, std::make_shared<logger>()); already test
	connection.on("ReceiveMessage", [](const web::json::value& m)
		{
			ucout << std::endl << m.at(0).as_string() << /*U(" wrote:") << m.at(1).as_string() <<*/ std::endl << U("Enter your message: ");
		});
	std::promise<void> task;
	connection.start([&connection, &task](std::exception_ptr exception)
		{
			if (exception)
			{
				try
				{
					std::rethrow_exception(exception);
				}
				catch (const std::exception& ex)
				{
					ucout << U("exception when starting connection: ") << ex.what() << std::endl;
				}
				task.set_value();
				return;
			}

			ucout << U("Enter your message:");
			std::string user = "AI Boss";
			for (;;)
			{
				std::string message;
				std::getline(std::cin, message);

				if (message == ":q")
				{
					break;
				}
				ZeusSendMessage(connection, user, message);
				ZeusSendMessageToCaller(connection, message);
			}

			connection.stop([&task](std::exception_ptr exception)
				{
					try
					{
						if (exception)
						{
							std::rethrow_exception(exception);
						}

						ucout << U("connection stopped successfully") << std::endl;
					}
					catch (const std::exception& e)
					{
						ucout << U("exception when stopping connection: ") << e.what() << std::endl;
					}

					task.set_value();
				});
		});

	task.get_future().get();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
